import React, { useContext, useState } from 'react';
import { useNavigate } from 'react-router-dom';
import '../style/Registration.css';
import PostService from '../API/PostService';
import { AuthContext } from '../context';
import sodium from 'libsodium-wrappers-sumo';
import {toast, ToastContainer} from 'react-toastify';

const Registration = () => {
    const [dataRegistration, setDataRegistration] = useState({
        firstname: 'Obito',
        lastname: 'Uchiha',
        is_male: 'true',
        birthdate: '1984-02-10',
        nickname: 'ObitoUchiha',
        email: 'Obito.Uchiha@konoha.jp',
        password: 'WithRin123',
        photo: null,
    });

    const navigate = useNavigate();

    const [dragActive, setDragActive] = useState(false);

    const handleChange = (e) => {
        const { name, value, files } = e.target;
        if (name === 'photo') {
            setDataRegistration({ ...dataRegistration, photo: files[0] });
        } else{
            setDataRegistration({ ...dataRegistration, [name]: value });
        }
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
            setDataRegistration({ ...dataRegistration, photo: e.dataTransfer.files[0] });
        }
    };

    const handleClick = () => {
        document.getElementById('photo').click();
    };

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

    const handleSendToServer = async (element) => {
        element.preventDefault();

        try {
            if (!dataRegistration.photo){
                throw new Error("Для успешной регистрации - осталось добавить фото");
            }
            console.log(dataRegistration.password);
            const password = await hashPassword(dataRegistration.password);
            console.log(password);

            const formData = new FormData();

            formData.append('first_name', dataRegistration.firstname);
            formData.append('last_name', dataRegistration.lastname);
            formData.append('is_male', dataRegistration.is_male);
            formData.append('birthdate', dataRegistration.birthdate);
            formData.append('nickname', dataRegistration.nickname);
            formData.append('email', dataRegistration.email);
            formData.append('password', password);
            formData.append('photo', dataRegistration.photo);

            const result = await PostService.registrServer(formData);

            console.log("Успех: ", result);
            localStorage.setItem("isAddNewUser", "true");
            navigate('/authorization');
        } catch (error) {
            toast.error(error.message);
        }

        // setDataRegistration({
        //     firstname: '',
        //     lastname: '',
        //     is_male: "true",
        //     birthdate: '',
        //     nickname: '',
        //     email: '',
        //     password: '',
        //     photo: null,
        // });
    };

    const handleRegisterClick = (e) => {
        e.preventDefault();
        navigate('/authorization');
    };

    return (
        <div className="registration-form">
            <ToastContainer/>
            <div ClassName="form-title"><h2>Регистрация</h2></div>
            <form onSubmit={handleSendToServer}>
                <div
                    className={`drop-zone in-registration ${dragActive ? 'active' : ''}`}
                    onDragOver={handleDragOver}
                    onDragLeave={handleDragLeave}
                    onDrop={handleDrop}
                    onClick={handleClick}
                >
                    <p>{dataRegistration.photo ? `Выбран файл: ${dataRegistration.photo.name}` : 'Перетащите фото сюда или нажмите для выбора'}</p>
                    <input type="file" id="photo" name="photo" accept="image/*" onChange={handleChange} style={{ display: 'none' }}/>
                </div>

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
                    <label htmlFor="password">Пароль:</label>
                    <input type="password" id="password" name="password" value={dataRegistration.password} onChange={handleChange} required />
                </div>

                <div className="form-group">
                    <input type="submit" value="Зарегистрироваться" />
                    <button onClick={handleRegisterClick}>Назад к авторизации</button>
                </div>
            </form>
        </div>
    );
}

export default Registration;
