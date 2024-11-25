import React from "react";
import { useState } from "react";
import PostService from "../../API/PostService";
import "./AddPost.css"
import { toast, ToastContainer } from 'react-toastify';

const AddPost = ({callback}) => {
    const [photoFile, setPhotoFile] = useState(null);
    const [dragActive, setDragActive] = useState(false);

    const [desription, setDesription] = useState("");


    const handleChange = (e) => {
        const { name, value, files } = e.target;
        if (name === 'photo')
            setPhotoFile(files[0]);
        else
            setDesription(value);
    };
    const handleDragOver = (e) => {
        e.preventDefault();
        e.stopPropagation();
        setDragActive(true);
    };
    const handleDragLeave = (e) => {
        e.preventDefault();
        e.stopPropagation();
        setDragActive(false);
    };
    const handleDrop = (e) => {
        e.preventDefault();
        e.stopPropagation();
        setDragActive(false);

        if (e.dataTransfer.files && e.dataTransfer.files[0]) {
            setPhotoFile(e.dataTransfer.files[0]);
        }
    };

    const handleClick = () => {
        document.getElementById('photo').click();
    };

    const handleSendToServerPhoto = async (element) => {
        try {
            element.preventDefault();

            if (!photoFile) {
                throw new Error("Выберите фото");
            }

            const formData = new FormData();

            formData.append('description', desription);
            formData.append('photo', photoFile);

            await PostService.addNewPost(formData);
            callback(true)
        }
        catch (error) {
            callback(false, error)
        }


    }

    return (
        <div className="container">
            <h2>Новый пост</h2>
            <form onSubmit={handleSendToServerPhoto}>
                <div className="form-group">
                    <input type="text" id="description" name="description" autocomplete="off" value={desription} onChange={handleChange}/>
                </div>
                <div className="form-group">
                    <div
                        className={`drop-zone ${dragActive ? 'active' : ''}`}
                        onDragOver={handleDragOver}
                        onDragLeave={handleDragLeave}
                        onDrop={handleDrop}
                        onClick={handleClick}
                    >
                        <p>{photoFile ? `Выбран файл: ${photoFile.name}` : 'Перетащите фото сюда или нажмите для выбора'}</p>
                        <input type="file" id="photo" name="photo" accept="image/*" onChange={handleChange} style={{ display: 'none' }} />
                    </div>
                </div>
                <div className="form-group">
                    <input type="submit" value="Опубилковать пост" />
                </div>
            </form>
        </div>
    )
}

export default AddPost;