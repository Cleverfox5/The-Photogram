import { React, useEffect, useState } from "react";
import Profile from "../components/Profile/Profile";
import PostService from "../API/PostService";
import { useParams } from "react-router-dom";

const posts = [
    {
        id: 1,
        photo: "https://upload.wikimedia.org/wikipedia/commons/5/55/Uzkoe2.jpg",
        description: "Это пример описания поста. Здесь можно написать любую информацию.",
        date: "15 ноября 2024",
        avatar: "https://via.placeholder.com/48",
        nickname: "example_user"
    },
    {
        id: 2,
        photo: "https://via.placeholder.com/600x600/ffff7f",
        description: "Это пример описания поста. Здесь можно написать любую информацию.",
        date: "15 ноября 2024",
        avatar: "https://via.placeholder.com/48",
        nickname: "example_user"
    },
    {
        id: 3,
        photo: "https://via.placeholder.com/300x500/7f7fff",
        description: "Это пример описания поста. Здесь можно написать любую информацию.",
        date: "15 ноября 2024",
        avatar: "https://via.placeholder.com/48",
        nickname: "example_user"
    },
    {
        id: 4,
        photo: "https://via.placeholder.com/300x500/7f7fff",
        description: "Это пример описания поста. Здесь можно написать любую информацию.",
        date: "15 ноября 2024",
        avatar: "https://via.placeholder.com/48",
        nickname: "example_user"
    },
    {
        id: 5,
        photo: "https://via.placeholder.com/600x400",
        description: "Очень длинное описание, которое должно быть обрезано, если превышает лимит текста. Очень длинное описание, которое должно быть обрезано, если превышает лимит текста. Очень длинное описание, которое должно быть обрезано, если превышает лимит текста. Очень длинное описание, которое должно быть обрезано, если превышает лимит текста. Очень длинное описание, которое должно быть обрезано, если превышает лимит текста. Очень длинное описание, которое должно быть обрезано, если превышает лимит текста. Очень длинное описание, которое должно быть обрезано, если превышает лимит текста. Очень длинное описание, которое должно быть обрезано, если превышает лимит текста. v v v Очень длинное описание, которое должно быть обрезано, если превышает лимит текста.Очень длинное описание, которое должно быть обрезано, если превышает лимит текста.Очень длинное описание, которое должно быть обрезано, если превышает лимит текста. Очень длинное описание, которое должно быть обрезано, если превышает лимит текста. Очень длинное описание, которое должно быть обрезано, если превышает лимит текста. Очень длинное описание, которое должно быть обрезано, если превышает лимит текста.  Очень длинное описание, которое должно быть обрезано, если превышает лимит текста.Очень длинное описание, которое должно быть обрезано, если превышает лимит текста.Очень длинное описание, которое должно быть обрезано, если превышает лимит текста.Очень длинное описание, которое должно быть обрезано, если превышает лимит текста.Очень длинное описание, которое должно быть обрезано, если превышает лимит текста. Очень длинное описание, которое должно быть обрезано, если превышает лимит текста.",
        date: "15 ноября 2024",
        avatar: "https://via.placeholder.com/48",
        nickname: "example_user"
    }
];

const UserProfile = () => {
    const { nickname } = useParams();
    const [user, setUser] = useState(null);
    const [urlPhotoUser, setUrlPhotoUser] = useState(null);
    const [status, setStatus] = useState('stranger');


    useEffect(() => {
        const fetchData = async () => {
            try {
                const userData = await PostService.getProfileInfByNick(nickname);
                if (nickname === localStorage.getItem('nickname')) {
                    setStatus('me');
                }
                else {
                    const result = await PostService.getStatus(userData.id);
                    const status = result.status;
                    setStatus(status);
                    console.log(`status: ${status}`);
                }
                const userPhoto = await PostService.getProfilePhotoByNickname(nickname);
                console.log(userPhoto);

                const urlPhotoUser = window.URL.createObjectURL(userPhoto);
                console.log('Generated URL: ', urlPhotoUser);

                setUser(userData);
                setUrlPhotoUser(urlPhotoUser);

                return () => window.URL.revokeObjectURL(urlPhotoUser);
            }
            catch (error) {
                console.error(error);
            }
        }

        fetchData();
    }, [nickname]);

    return (
        <Profile user={user} urlPhotoUser={urlPhotoUser} posts={posts} status_={status} />
    );
}

export default UserProfile;