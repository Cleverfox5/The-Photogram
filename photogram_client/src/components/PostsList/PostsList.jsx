import React from "react";
import Post from "../Post/Post";


const PostsList = (posts, status_, to_ready_callback) => {

    return (
        <div className="posts-page-container">
            {posts.length === 0
                ?
                <p className="no-posts">Добавьте пользователей в друзья, чтобы видеть их посты</p>
                :
                posts.map((post) =>
                    <Post postData={post} status={status_} to_ready_callback={to_ready_callback} className="post-item" />
                )
            }

            <div ref={paginationElement} style={{ height: 20, background: 'white' }}></div>
        </div>
    );
}

export default PostsList;