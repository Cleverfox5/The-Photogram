import React, { useEffect, useState } from 'react';
import { Route, Routes, Navigate, BrowserRouter } from "react-router-dom";
import Navbar from './components/UI/Navbar/Navbar';
import './style/style.css'
import './style/messagerStyle.css'
import Messenger from './pages/Messenger';
import Authorization from './pages/Authorization';
import ErrorPage from './pages/ErrorPage';
import Router from './components/UI/Router';
import { AuthContext } from './context';

function App() {
  const [isAuth, setIsAuth] = useState(false);
  const [isLoading, setIsLoading] = useState(true);

  useEffect(() => {
    if (localStorage.getItem('auth')) {
      setIsAuth(true)
    }
    setIsLoading(false)
  }, [])

  return (
    <AuthContext.Provider value={{
      isAuth,
      setIsAuth,
      isLoading
    }}>
      <div className='content'>
        {isAuth
          ?
          <BrowserRouter>
            <div className='nav'>
              <Navbar />
            </div>
            <div className='page'>
              <Router />
            </div>
          </BrowserRouter>
          :
          <BrowserRouter>
            <div className='page'>
              <Router />
            </div>
          </BrowserRouter>
        }
      </div>
    </AuthContext.Provider>
  );
}

export default App;
