import React from "react";
import './Loading.modle.css';
import loadingImage from './resurs/loading.png';

const Loading = () => {
    return (
        <div className='loading-spinner'>
            <img src={loadingImage} alt="Loading"/>
        </div>
    )
}
export default Loading;