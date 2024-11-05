import React, { useContext } from "react";
import MyButton from "../button/MyButton";
import './Navbar.css'
import { useNavigate } from "react-router-dom";
import { AuthContext } from "../../../context";

const Navbar = () => {
    const {isAuth, setIsAuth} = useContext(AuthContext);
    const navigate = useNavigate();
    const logout = () => {
        navigate('/authorization');
        setIsAuth(false);
        localStorage.removeItem('auth')
        localStorage.removeItem('refreshToken')
        localStorage.removeItem('nickname')
    }
    return (
        <div className='navbar'>
            <div className='top'>
                <MyButton onClick = {() => navigate(`/profile/${localStorage.getItem('nickname')}`)}>Я</MyButton>
                <MyButton onClick = {() => navigate('/news')}>Лента</MyButton>
                <MyButton onClick = {() => navigate('/friendSearch')}>Найти друга</MyButton>
            </div>
            <div className='bottom'>
                <MyButton onClick = {() => logout()}>Выйти</MyButton>
            </div>
        </div>
    )
}

export default Navbar