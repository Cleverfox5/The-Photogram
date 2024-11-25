import React, { useState, useEffect } from "react";
import "./Post.css";
import binIcon from "./resources/bin.png"
import likeIcon from "./resources/like.png";
import { useNavigate } from "react-router-dom";
import PostService from "../../API/PostService";

const Post = ({ postData, status, remove, to_ready_callback }) => {
  const [photoUrl, setPhotoUrl] = useState(null);
  const [profilePhotoUrl, setProfilePhotoUrl] = useState(null);
  const [isLargeImage, setIsLargeImage] = useState(false);
  const [isLiked, setIsLiked] = useState(postData.is_liked === 'true');
  const [likesCount, setLikesCount] = useState(Number(postData.like_count));
  const [isAddedFirst, setIsAddedFirst] = useState(false);

  useEffect(() => {
    if (isAddedFirst){
      to_ready_callback()
    }
  }, [isAddedFirst])

  const handleImageLoad = (e) => {
    if (e.target.naturalHeight > 400) {
      setIsLargeImage(true);
    }
  };

  const navigate = useNavigate();

  useEffect(() => {
    const fetchPhoto = async () => {
      const blob = await PostService.getPostPhoto(postData.photo_id);
      const imgURL = URL.createObjectURL(blob);
      setPhotoUrl(imgURL);
      console.log("Добавилось фото, состояните сменилось на true")
      setIsAddedFirst(true);
    };

    fetchPhoto();
  }, [postData.photo_id])

  useEffect(() => {
    const fetchPhoto = async () => {
      const blob = await PostService.getPhotoById(postData.profile_picture_id);
      const imgURL = URL.createObjectURL(blob);
      setProfilePhotoUrl(imgURL);
    };

    fetchPhoto();
  }, [postData.profile_picture_id])

  const handleLike = async (element) => {
    element.preventDefault();
    const is_ckick_success = await PostService.postLike(postData.post_id);
    console.log(isLiked)
    if (isLiked) {
      setIsLiked(false);
      setLikesCount(likesCount - 1)
    }
    else {
      setIsLiked(true);
      setLikesCount(likesCount + 1)
    }
  };
  const handleBin = async () => {
    try {
      await PostService.deletePost(postData.post_id);
      remove(postData.post_id)
    }
    catch (e) {
      
    }
  };

  const DateForm = (date_string) => {
    const months = [
      'January', 'February', 'March', 'April', 'May', 'June',
      'July', 'August', 'September', 'October', 'November', 'December'
    ];

    const date = new Date(date_string)
    const day = date.getDate()
    const month = months[date.getMonth()]
    const year = date.getFullYear()
    const hours = date.getHours()
    const minute = date.getMinutes()

    const curr_date = new Date()

    if (year === curr_date.getFullYear()) return `${String(day).padStart(2, '0')} ${month} ${String(hours).padStart(2, '0')}:${String(minute).padStart(2, '0')}`
    else return `${String(day).padStart(2, '0')} ${month} ${year} ${String(hours).padStart(2, '0')}:${String(minute).padStart(2, '0')}`
  }


  return (
    <div>
      {isAddedFirst
        ? <div className="post">
        <div className="post-header">
          <div className="post-header-left" onClick={() => navigate(`/profile/${postData.nickname}`)}>
            <img src={profilePhotoUrl} alt={`${postData.nickname}'s avatar`} className="post-avatar" />
            <div className="post-info">
              <span className="post-nickname">{postData.nickname}</span>
              <span className="post-date">
                {postData.date_posted && DateForm(postData.date_posted)}
              </span>
            </div>
          </div>
          {status === 'me'
            ?
            <button className="bin-button" onClick={handleBin}>
              <img src={binIcon}
                alt="bin"
                className="bin" />
            </button>
            : <></>
          }
        </div>
        <p className="post-description">{postData.description}</p>
        <div
          className="post-photo-container"
          style={{ height: isLargeImage ? "400px" : "auto" }}
        >
          <img
            src={photoUrl}
            alt="Post content"
            className="post-photo"
            onLoad={handleImageLoad}
            style={{ objectFit: isLargeImage ? "contain" : "none" }}
          />
        </div>
        <div className="post-footer">
          <button className={isLiked ? "like-button clicked" : "like-button"} onClick={handleLike}>
            <img src={likeIcon}
              alt="like"
              className="like" /> {likesCount === 0 ? '' : likesCount}
          </button>
        </div>
      </div>
      : <></>
      }
    </div>
  );
};

export default Post;
