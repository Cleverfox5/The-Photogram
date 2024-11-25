import React, { useState, useEffect, useRef } from 'react';
import '../style/PostsPage.css';
import Post from '../components/Post/Post';
import PostService from '../API/PostService';
import Loading from '../components/UI/loading/Loading'
const PostsPage = () => {
  const [settings, setSettings] = useState({ limit: 2, offset: -2 });
  const [posts, setPosts] = useState([]);
  const [isEnough, setIsEnough] = useState(false);
  const [isLoading, setIsLoading] = useState(true);
  const paginationElement = useRef();
  const observer = useRef();

  const to_ready_callback = () => {
    setIsLoading(false)
  }

  useEffect(() => {
    const callback = function (entries, observer) {
      if (!isEnough && entries[0].isIntersecting) {
        setIsLoading(true);
        loadMorePosts();
      }
    };
    observer.current = new IntersectionObserver(callback);
    if (paginationElement.current) {
      observer.current.observe(paginationElement.current);
    } else {
      console.error("paginationElement.current is not defined");
    }
    return () => {
      if (paginationElement.current) {
        observer.current.unobserve(paginationElement.current);
      }
      observer.current.disconnect();
    }
  }, [isEnough, posts])

  const loadMorePosts = async () => {
    setIsLoading(true)
    const postsList = await PostService.getPostsList(settings.limit, settings.offset + settings.limit);
    if (!postsList) {
      setIsEnough(true);
      setIsLoading(false);
    }
    else {
      if (postsList.length < settings.limit) {
        setIsEnough(true);
      }
      if (!postsList.length) setIsLoading(false)
      setPosts([...posts, ...postsList]);
      setSettings({ ...settings, offset: settings.offset + settings.limit });
    }
  }

  return (
    <div className="posts-page-container">
      {!isLoading && posts.length === 0
            ?
            <p className="no-posts">Добавьте пользователей в друзья, чтобы видеть их посты</p>
            :
            posts.map((post) =>
              <Post postData={post} status={'staranger'} to_ready_callback={to_ready_callback} className="post-item" />
            )
      }
      {isLoading &&
        <Loading />
      }

      <div ref={paginationElement} style={{ height: 20, background: 'white' }}></div>
    </div>
  );
};

export default PostsPage;
