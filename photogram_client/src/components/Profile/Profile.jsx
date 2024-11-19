import React, { useState, useEffect, useRef } from "react";
import './Profile.css';
import PostService from "../../API/PostService";
import { toast, ToastContainer } from "react-toastify";
import sodium from 'libsodium-wrappers-sumo';
import Post from "../Post/Post";
import AddPost from "../AddPost/AddPost";

const Profile = ({ user, urlPhotoUser, status_ }) => {
    const [status, setStatus] = useState(status_);
    const [isChangeProfileClick, setIsChangeProfileClick] = useState(false);
    const [mode, setMode] = useState('page');
    const [text_from_second_server, setTextFromSecondServer] = useState("");

    const [settings, setSettings] = useState({ limit: 1, offset: 0 });
    const [posts, setPosts] = useState([]);
    const [isEnough, setIsEnough] = useState(false);
    const [isFirst, setIsFirst] = useState(true);
    const paginationElement = useRef();
    const observer = useRef();

    useEffect(() => {
        const callback = function (entries, observer) {
            if (!isFirst && !isEnough && entries[0].isIntersecting) {
                loadMorePosts();
            }
        };
        observer.current = new IntersectionObserver(callback);
        if (paginationElement.current) {
            observer.current.observe(paginationElement.current);
        } else {
            console.error("paginationElement.current is not defined");
        }
        return () => {
            if (paginationElement.current) {
                observer.current.unobserve(paginationElement.current);
            }
            observer.current.disconnect();
        }
    }, [isEnough, posts])

    const loadMorePosts = async () => {
        console.log(settings.limit)
        const postsList = await PostService.getPostsList(settings.limit, settings.offset + settings.limit);
        if (!postsList) {
            setIsEnough(true);
        }
        else {
            if (postsList.length < settings.limit) {
                setIsEnough(true);
            }
            setPosts([...posts, ...postsList]);
            setSettings({ ...settings, offset: settings.offset + settings.limit });
        }
    }

    const loadPosts = async (element) => {
        element.preventDefault();
        setIsFirst(true);
        try {
            setPosts([]);
            setIsEnough(false);
            console.log(settings);
            const postsList = await PostService.getPostsList(settings.limit, 0);
            if (postsList) {
                setPosts(postsList);
                console.log(postsList);
                if (postsList.length < settings.limit) {
                    setIsEnough(true);
                }

                setSettings({ ...settings, offset: 0 });
                setIsFirst(false);
            }
        }
        catch (error) {
            console.log(error.message);
        }
        console.log(posts);
    }







    const [dataRegistration, setDataRegistration] = useState({
        firstname: '',
        lastname: '',
        is_male: "true",
        birthdate: '',
        nickname: '',
        email: ''
    });

    const [newPassword, setNewPassword] = useState('');
    const [photoFile, setPhotoFile] = useState(null);

    const [dragActive, setDragActive] = useState(false);

    const handleChange = (e) => {
        const { name, value, files } = e.target;
        if (name === 'photo')
            setPhotoFile(files[0]);
        else if (name === 'password')
            setNewPassword(value);
        else
            setDataRegistration({ ...dataRegistration, [name]: value });
    };

    const handleDragOver = (e) => {
        e.preventDefault();
        e.stopPropagation();
        setDragActive(true);
    };

    const handleDragLeave = (e) => {
        e.preventDefault();
        e.stopPropagation();
        setDragActive(false);
    };

    const handleDrop = (e) => {
        e.preventDefault();
        e.stopPropagation();
        setDragActive(false);

        if (e.dataTransfer.files && e.dataTransfer.files[0]) {
            setPhotoFile(e.dataTransfer.files[0]);
        }
    };

    const handleClick = () => {
        document.getElementById('photo').click();
    };


    useEffect(() => {
        setStatus(status_);
    }, [status_])

    const delFriend = async (event) => {
        event.stopPropagation();
        await PostService.deleteFriend(user.id)
        setStatus('stranger');
    }

    const addFriend = async (event) => {
        event.stopPropagation();
        await PostService.makeFriend(user.id)
        setStatus('friend');
        const getHelloWorld = async () => {
            console.log("В http запросе строчка: " + await PostService.getDate())
        }
        getHelloWorld()
    }

    const changeProfile = () => {
        console.log("Здесь будет изменение профиля")
        setIsChangeProfileClick(true);
        setMode('menu');
        setDataRegistration({
            firstname: user.first_name,
            lastname: user.last_name,
            is_male: user.is_male,
            birthdate: user.birthdate,
            nickname: user.nickname,
            email: user.email,
        })
    }

    const handleBackClick = () => {
        if (mode === 'menu') setMode('page')
        else {
            setPhotoFile(null);
            setMode('menu');
        }
    }

    const handleSendToServerPhoto = async (element) => {
        element.preventDefault();
        try {
            if (!photoFile) {
                throw new Error("Выберите фото");
            }

            const formData = new FormData();

            formData.append('photo', photoFile);

            await PostService.updateProfilePhoto(formData);

            setMode('page')
            document.location.reload();
        } catch (error) {
            toast.error(error.message);
        }
    }

    const hashPassword = async (password) => {
        await sodium.ready
        const hashedPassword = sodium.crypto_pwhash_str(
            password,
            sodium.crypto_pwhash_OPSLIMIT_INTERACTIVE,
            sodium.crypto_pwhash_MEMLIMIT_INTERACTIVE
        );


        console.log(hashedPassword);

        return hashedPassword;
    };

    const handleSendToServerData = async (element) => {
        element.preventDefault();
        try {
            await PostService.updateProfileData(dataRegistration);
            if (newPassword !== "") {
                const password = await hashPassword(newPassword);

                await PostService.updateProfilePassword(password);
            }
            setMode('page')
            document.location.reload();
        } catch (error) {
            toast.error(error.message);
        }
    }

    const hanldeGetIdFromAnotherServer = async (event) => {
        event.stopPropagation();
        const getHelloWorld = async () => {
            setTextFromSecondServer(await PostService.getDate())
        }
        getHelloWorld()
    }

    if (!user || !urlPhotoUser) {
        return <div>Loading profile...</div>;
    }

    return (
        <div className="profile-container">
            <ToastContainer />
            {mode === 'page' && (
                <div>
                    <div className="profile-header">
                        <img src={urlPhotoUser} alt="Фото профиля" className="profile-photo" />
                        <h1 className="profile-name">{user.first_name} {user.last_name}</h1>
                    </div>
                    <div className="profile-details">
                        <p><strong>Пол: </strong>
                            {user.is_male === true
                                ? <>Мужчина</>
                                : <>Женщина</>
                            }</p>
                        <p><strong>Дата рождения:</strong> {new Date(user.birthdate).toLocaleDateString()}</p>
                    </div>
                    <div className="profile-buttons">
                        {status === 'stranger'
                            ? <button onClick={addFriend} className="add-btn">Добавить в друзья</button>
                            :
                            status === 'friend'
                                ? <button onClick={delFriend} className="del-btn">Удалить из друзей</button>
                                : <button onClick={changeProfile} className="edit-profile-btn">Редактировать профиль</button>
                        }
                        <button onClick={hanldeGetIdFromAnotherServer}>Get Id</button>
                    </div>
                    {status === 'me'
                        ? <AddPost />
                        : <p></p>
                    }
                    <div>
                        <p>{text_from_second_server}</p>
                    </div>

                    <button onClick={loadPosts} className="search-button">
                        Поиск
                    </button>

                    <div className="posts-section">
                        <h2>Посты</h2>
                        {posts.length === 0
                            ?
                            <p className="no-posts">Нет ни одного поста</p>
                            :
                            posts.map(post =>
                                <Post postData={post} status={status} className="post-item" />
                            )
                        }
                        <div ref={paginationElement} style={{ height: 20, background: 'white' }}></div>
                    </div>
                </div>
            )}
            {isChangeProfileClick && (
                <div className="change-profile-page">
                    <div className="change-profile-content">
                        {mode === 'menu' && (
                            <div className="short menu">
                                <button onClick={() => setMode('photo')}>
                                    Изменить фото
                                </button>
                                <button onClick={() => setMode('data')}>
                                    Изменить данные
                                </button>
                                <div className="form-group">
                                    <button onClick={handleBackClick}>Назад</button>
                                </div>
                            </div>
                        )}

                        {mode === 'photo' && (
                            <div className="short add-new-photo">
                                <div className="form-title"><h2>Изменение фото</h2></div>
                                <form onSubmit={handleSendToServerPhoto}>
                                    <div
                                        className={`drop-zone ${dragActive ? 'active' : ''}`}
                                        onDragOver={handleDragOver}
                                        onDragLeave={handleDragLeave}
                                        onDrop={handleDrop}
                                        onClick={handleClick}
                                    >
                                        <p>{photoFile ? `Выбран файл: ${photoFile.name}` : 'Перетащите фото сюда или нажмите для выбора'}</p>
                                        <input type="file" id="photo" name="photo" accept="image/*" onChange={handleChange} style={{ display: 'none' }} />
                                    </div>
                                    <div className="form-group">
                                        <input type="submit" value="Изменить фото" />
                                        <button onClick={handleBackClick}>Назад</button>
                                    </div>
                                </form>
                            </div>
                        )}
                        {mode === 'data' && (
                            <div className="short">
                                <div className="form-title"><h2>Изменение данных</h2></div>
                                <form onSubmit={handleSendToServerData}>
                                    <div className="form-group">
                                        <label htmlFor="firstname">Имя:</label>
                                        <input type="text" id="firstname" name="firstname" value={dataRegistration.firstname} onChange={handleChange} required />
                                    </div>

                                    <div className="form-group">
                                        <label htmlFor="lastname">Фамилия:</label>
                                        <input type="text" id="lastname" name="lastname" value={dataRegistration.lastname} onChange={handleChange} required />
                                    </div>

                                    <div className="form-group">
                                        <label htmlFor="gender">Пол:</label>
                                        <select id="gender" name="is_male" value={dataRegistration.is_male} onChange={handleChange} required >
                                            <option value="true">Мужской</option>
                                            <option value="false">Женский</option>
                                        </select>
                                    </div>

                                    <div className="form-group">
                                        <label htmlFor="birthdate">Дата рождения:</label>
                                        <input type="date" id="birthdate" name="birthdate" value={dataRegistration.birthdate} onChange={handleChange} required />
                                    </div>

                                    <div className="form-group">
                                        <label htmlFor="nickname">Никнейм:</label>
                                        <input type="text" id="nickname" name="nickname" value={dataRegistration.nickname} onChange={handleChange} required />
                                    </div>

                                    <div className="form-group">
                                        <label htmlFor="email">Email:</label>
                                        <input type="email" id="email" name="email" value={dataRegistration.email} onChange={handleChange} required />
                                    </div>

                                    <div className="form-group">
                                        <label htmlFor="password">Новый пароль:</label>
                                        <input type="password" id="password" name="password" value={dataRegistration.password} onChange={handleChange} />
                                    </div>

                                    <div className="form-group">
                                        <input type="submit" value="Изменить данные" />
                                        <button onClick={handleBackClick}>Назад</button>
                                    </div>
                                </form>
                            </div>
                        )}
                    </div>
                </div>
            )}
        </div>
    );
}

export default Profile;