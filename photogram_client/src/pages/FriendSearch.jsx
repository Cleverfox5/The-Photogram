import React, { useState, useEffect, useRef } from "react";
import "../style/FriendSearch.css";
import FindFriendItem from "../components/FindFriendItem/FindFriendItem";
import axios from "axios";
import PostService from "../API/PostService";
import { toast, ToastContainer } from 'react-toastify';

const FriendSearch = () => {
  const [settings, setSettings] = useState({ substr: '', limit: 5, offset: 0 });
  const [users, setUsers] = useState([]);
  const [isEnough, setIsEnough] = useState(false);
  const [isFirst, setIsFitst] = useState(true);
  const paginationElement = useRef();
  const observer = useRef();

  useEffect(() => {
    const callback = function (entries, observer) {
      if (!isFirst && !isEnough && entries[0].isIntersecting) {
        loadMoreUsers();
      }
    };
    observer.current = new IntersectionObserver(callback);
    observer.current.observe(paginationElement.current)
    return () => {
      if (paginationElement.current) {
        observer.current.unobserve(paginationElement.current);
      }
      observer.current.disconnect();
    }
  }, [isEnough, users])

  const loadMoreUsers = async () => {
    const usersList = await PostService.getUsersList(settings.substr, settings.limit, settings.offset + settings.limit);
    if (!usersList) {
      setIsEnough(true);
    }
    else {
      if (usersList.length < settings.limit) {
        setIsEnough(true);
      }
      setUsers([...users, ...usersList]);
      setSettings({ ...settings, offset: settings.offset + settings.limit });
    }
  }

  const loadUsers = async (element) => {
    element.preventDefault();
    setIsFitst(true);
    try {
      setUsers([]);
      setIsEnough(false);
      console.log(settings);
      const usersList = await PostService.getUsersList(settings.substr, settings.limit, 0);
      if (usersList) {
        setUsers(usersList);
        console.log(usersList);
        if (usersList.length < settings.limit) {
          setIsEnough(true);
        }

        setSettings({ ...settings, offset: 0 });
        setIsFitst(false);
      }
    }
    catch (error) {
      console.log(error.message);
    }
    console.log(users);
  }

  return (
    <div className="friend-search-container">
      <h2 className="title">Найти друзей</h2>
      <div className="search-bar">
        <form>
          <input
            type="text"
            onChange={(e) => setSettings({ ...settings, substr: e.target.value, offset: 0 })}
            placeholder="Введите никнейм друга"
            className="search-input"
            required
          />
          <button onClick={loadUsers} className="search-button">
            Поиск
          </button>
        </form>
      </div>

      <div className="friend-list">
        {users.length === 0
          ?
          <p className="no-friends">Не найдино ни одного пользователя</p>
          :
          users.map(user =>
            <FindFriendItem friend={user} />
          )
        }
        <div ref={paginationElement} style={{ height: 20, background: 'white' }}></div>
      </div>
    </div>
  );
};

export default FriendSearch;