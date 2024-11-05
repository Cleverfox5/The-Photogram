import React, { useContext, useEffect, useState } from "react";
import MyButton from "../components/UI/button/MyButton";
import MyInput from "../components/UI/input/MyInput";
import "../style/authorization.css"
import { AuthContext } from "../context";
import { Navigate, useNavigate } from "react-router-dom";
import PostService from "../API/PostService";

import { toast, ToastContainer } from 'react-toastify';
import 'react-toastify/dist/ReactToastify.css';

const Authorization = () => {
    const [dataAuth, setDataAuth] = useState({login: 'Utiho03@mail.ru', password: 'Utiho123'});
    const {isAuth, setIsAuth} = useContext(AuthContext);
    const { isAddedNewUser, setisAddedNewUser} = useContext(AuthContext);
    
    const navigator = useNavigate();

    useEffect(() => {
        const isAdded = () => {
            console.log(`Да, я здесь, мне здесь хорошо - ${localStorage.getItem('isAddNewUser')}`);
            if (localStorage.getItem('isAddNewUser') === 'true'){
                toast.success("Добавлен новый пользователь");
                localStorage.removeItem('isAddNewUser');
            }
        }
        isAdded();
    }, []);

    const ChekAuthtoirsation = async (element) => {
        element.preventDefault();

        try {
            const result = await PostService.authServer(dataAuth);
            
            console.log("Успех: ", result);
            localStorage.setItem('auth', 'true')
            setIsAuth("true");
        } catch (error) {
            toast.error(error.message);
        }

        //setDataAuth({login: '', password: ''});
    };

    const toRegistrationPage = (element) => {
        element.preventDefault();
        navigator('/registration');
    };

    return(
        <div>
            <ToastContainer />
            <form className="auth" onSubmit={ChekAuthtoirsation}>
                <h1>Авторизация</h1>
                <MyInput type="text" value={dataAuth.login} placeholder="Введите логин" onChange={e => setDataAuth({...dataAuth, login: e.target.value})} required/>
                <MyInput type="password" value={dataAuth.password} placeholder="Введите проль" onChange={e => setDataAuth({...dataAuth, password: e.target.value})} required/>
                <MyButton >Войти</MyButton>
                <MyButton onClick={toRegistrationPage}>Зарегистрироваться</MyButton>
            </form>
        </div>
    )
}

export default Authorization;