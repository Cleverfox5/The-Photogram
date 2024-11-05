import { React, useEffect, useState } from "react";
import Profile from "../components/Profile/Profile";
import PostService from "../API/PostService";
import { useParams } from "react-router-dom";


const posts = [
    { id: 1, src: 'https://via.placeholder.com/600x400/ff7f7f', alt: 'Пост 1' },
    { id: 2, src: 'https://via.placeholder.com/300x500/7f7fff', alt: 'Пост 2' },
    { id: 3, src: 'https://via.placeholder.com/800x300/7fff7f', alt: 'Пост 3' },
    { id: 4, src: 'https://via.placeholder.com/600x600/ffff7f', alt: 'Пост 4' }
];

const getHelloWorld = async () => {
    console.log("В http запросе строчка: " + await PostService.getDate())
}

const UserProfile = () => {
    const { nickname } = useParams();
    const [user, setUser] = useState(null);
    const [urlPhotoUser, setUrlPhotoUser] = useState(null);
    const [status, setStatus] = useState('stranger');

    
    getHelloWorld()
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