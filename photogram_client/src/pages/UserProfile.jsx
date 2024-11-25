import { React, useEffect, useState } from "react";
import Profile from "../components/Profile/Profile";
import PostService from "../API/PostService";
import { useParams } from "react-router-dom";


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
        <Profile user={user} urlPhotoUser={urlPhotoUser} status_={status} />
    );
}

export default UserProfile;