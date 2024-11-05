
import React, { useEffect, useState } from 'react';
import './FindFriendItem.css';
import PostService from '../../API/PostService';
import { useNavigate } from 'react-router-dom';

const FindFriendItem = ({ friend }) => {
  const [photoUrl, setPhotoUrl] = useState(null);
  
  let boolval = false;
  if (friend.is_friend === 'true')
    boolval = true;

  const [isFriend, setIsFriend] = useState(boolval);

  const navigate = useNavigate();

  useEffect(() => {
    const fetchPhoto = async () => {
      const blob = await PostService.getPhotoById(friend.photo_id);
      const imgURL = URL.createObjectURL(blob);
      setPhotoUrl(imgURL);
    };

    fetchPhoto();
  }, [friend.photo_id])

  const сhangePersonStatus = async (event) => {
    event.stopPropagation();
    if (isFriend){
      console.log("Удаление друга");
      await PostService.deleteFriend(friend.friend_id)
    }
    else{
      console.log("Добавление друга");
      await PostService.makeFriend(friend.friend_id)
    }
    setIsFriend((prevIsFriend) => !prevIsFriend);
  }

  return (
      <div className="friend-card" onClick={() => navigate(`/profile/${friend.nickname}`)}>
        {photoUrl
          ?
          <img src={photoUrl} alt={friend.nickname} className="friend-photo" />
          :
          <div className="friend-photo">...</div>
        }
        <div className="friend-info">
          <h3 className="friend-name">{friend.nickname}</h3>
          <button
            onClick={сhangePersonStatus}
            className={isFriend ? "delete-friend-button" : "add-friend-button"}
          >
            {isFriend ? "Удалить" : "Добавить"}
          </button>
        </div>
      </div>
  );
};

export default FindFriendItem;

