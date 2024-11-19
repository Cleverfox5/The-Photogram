import { toast, ToastContainer } from 'react-toastify';
import 'react-toastify/dist/ReactToastify.css';
export default class PostService {
    static async getUsersList(substr, limit, offset){
        try{
            let response = await fetch(`http://localhost:8080/getUsersList?substr=${substr}&LIMIT=${limit}&OFFSET=${offset}`, {
                method: "GET",
                headers: {
                    'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                }
            });

            if (response.status === 401) {
                const accessToken = await this.refreshAccessToken();

                if (!accessToken) {
                    throw new Error('RefreshToken is NULL')
                }

                response = await fetch(`http://localhost:8080/getUsersList?substr=${substr}&LIMIT=${limit}&OFFSET=${offset}`, {
                    method: "GET",
                    headers: {
                        'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                    }
                });
            }
            console.log(response);

            if (!response.ok) {
                throw new Error(`Ошибка: ${response.status}`);
            }
            
            return await response.json();
        } catch (error) {
            console.log(error)
            throw error;
        }
    }

    static async getStatus(id_user){
        try{
            let response = await fetch(`http://localhost:8080/getStatus?id_user=${id_user}`, {
                method: "GET",
                headers: {
                    'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                }
            });
            console.log(response);

            if (response.status === 401) {
                const accessToken = await this.refreshAccessToken();

                if (!accessToken) {
                    throw new Error('RefreshToken is NULL')
                }

                response = await fetch(`http://localhost:8080/getStatus?id_user=${id_user}`, {
                    method: "GET",
                    headers: {
                        'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                    }
                });
                console.log(response);
            }

            if (!response.ok) {
                throw new Error(`Ошибка: ${response.status}`);
            }

            return await response.json();
        } catch (error) {
            console.log(error)
            throw error;
        }
    }

    static async makeFriend(friend_id){
        try{
            let response = await fetch(`http://localhost:8080/makeFriend?friend_id=${friend_id}`, {
                method: "GET",
                headers: {
                    'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                }
            });
            console.log(response);

            if (response.status === 401) {
                const accessToken = await this.refreshAccessToken();

                if (!accessToken) {
                    throw new Error('RefreshToken is NULL')
                }

                response = await fetch(`http://localhost:8080/makeFriend?friend_id=${friend_id}`, {
                    method: "GET",
                    headers: {
                        'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                    }
                });
                console.log(response);
            }

            if (!response.ok) {
                throw new Error(`Ошибка: ${response.status}`);
            }
        } catch (error) {
            console.log(error)
            throw error;
        }
    }

    static async deleteFriend(friend_id){
        try{
            let response = await fetch(`http://localhost:8080/deleteFriend?friend_id=${friend_id}`, {
                method: "GET",
                headers: {
                    'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                }
            });
            console.log(response);

            if (response.status === 401) {
                const accessToken = await this.refreshAccessToken();

                if (!accessToken) {
                    throw new Error('RefreshToken is NULL')
                }

                response = await fetch(`http://localhost:8080/deleteFriend?friend_id=${friend_id}`, {
                    method: "GET",
                    headers: {
                        'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                    }
                });
                console.log(response);
            }

            if (!response.ok) {
                throw new Error(`Ошибка: ${response.status}`);
            }
            
        } catch (error) {
            console.log(error)
            throw error;
        }
    }
    
    static async getPhotoById(id){
        try{
            let response = await fetch(`http://localhost:8080/getPhotoById?photo_id=${id}`, {
                method: "GET",
                headers: {
                    'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                }
            });

            if (response.status === 401) {
                const accessToken = await this.refreshAccessToken();

                if (!accessToken) {
                    throw new Error('RefreshToken is NULL')
                }

                response = await fetch(`http://localhost:8080/getPhotoById?photo_id=${id}`, {
                    method: "GET",
                    headers: {
                        'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                    }
                });
            }
            console.log(response);

            if (!response.ok) {
                throw new Error(`Ошибка: ${response.status}`);
            }

            return await response.blob();
        } catch (error) {
            console.log(error)
            throw error;
        }
    }

    static async authServer(dataAuth) {
        try {
            let response = await fetch("http://localhost:8080/login", {
                method: "POST",
                headers: {
                    "Content-Type": "application/json",
                },
                body: JSON.stringify(dataAuth),
                credentials: 'include'
            });

            console.log(response);

            if (!response.ok) {
                let errorData = {};

                try{
                    errorData = await response.json();
                    console.log('всё разджейсонилось');
                }
                catch (error){
                    console.log('Ошибка');
                    throw new Error(`${response.status}: Неизвестная ошибка`)
                }
                console.log(`${response.status}: ${errorData.error}`);
                throw new Error(`${response.status}: ${errorData.error}`);
            }

            const result = await response.json();

            localStorage.setItem("accessToken", result.accessToken);
            localStorage.setItem("nickname", result.nickname);

            return result;
        } catch (error) {
            throw error;
        }
    }
    static async updateProfileData(data){
        try{
            let response = await fetch(`http://localhost:8080/updateProfileData`, {
                method: "POST",
                headers: {
                    'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                },
                body: JSON.stringify(data),
            });
            console.log(response);

            if (response.status === 401) {
                const accessToken = await this.refreshAccessToken();

                if (!accessToken) {
                    throw new Error('RefreshToken is NULL')
                }

                response = await fetch(`http://localhost:8080/updateProfileData`, {
                    method: "POST",
                    headers: {
                        'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                    },
                    body: JSON.stringify(data),
                });
                console.log(response);
            }

            if (!response.ok) {
                throw new Error(`Ошибка: ${response.status}`);
            }
            
        } catch (error) {
            console.log(error)
            throw error;
        }
    }

    static async updateProfilePassword(data){
        try{
            let response = await fetch(`http://localhost:8080/updateProfilePassword`, {
                method: "POST",
                headers: {
                    'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                },
                body: JSON.stringify(data),
            });
            console.log(response);

            if (response.status === 401) {
                const accessToken = await this.refreshAccessToken();

                if (!accessToken) {
                    throw new Error('RefreshToken is NULL')
                }

                response = await fetch(`http://localhost:8080/updateProfilePassword`, {
                    method: "POST",
                    headers: {
                        'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                    },
                    body: JSON.stringify(data),
                });
                console.log(response);
            }

            if (!response.ok) {
                throw new Error(`Ошибка: ${response.status}`);
            }
            
        } catch (error) {
            console.log(error)
            throw error;
        }
    }

    static async authServer(dataAuth) {
        try {
            let response = await fetch("http://localhost:8080/login", {
                method: "POST",
                headers: {
                    "Content-Type": "application/json",
                },
                body: JSON.stringify(dataAuth),
                credentials: 'include'
            });

            console.log(response);

            if (!response.ok) {
                let errorData = {};

                try{
                    errorData = await response.json();
                    console.log('всё разджейсонилось');
                }
                catch (error){
                    console.log('Ошибка');
                    throw new Error(`${response.status}: Неизвестная ошибка`)
                }
                console.log(`${response.status}: ${errorData.error}`);
                throw new Error(`${response.status}: ${errorData.error}`);
            }

            const result = await response.json();

            localStorage.setItem("accessToken", result.accessToken);
            localStorage.setItem("nickname", result.nickname);

            return result;
        } catch (error) {
            throw error;
        }
    }

    static async registrServer(dataAuth) {
        try {
            let response = await fetch("http://localhost:8080/authorization", {
                method: "POST",
                body: dataAuth, // уже FormData, а не json
            });

            if (!response.ok) {
                const errorData = await response.json();
                throw new Error(`${response.status}: ${errorData.error}`);
            }
        } catch (error) {
            console.log(error);
            throw error;
        }
    }
    
    static async updateProfilePhoto(dataAuth){
        try{
            let response = await fetch(`http://localhost:8080/updateProfilePhoto`, {
                method: "POST",
                headers: {
                    'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                },
                body: dataAuth,
            });
            console.log(response);

            if (response.status === 401) {
                const accessToken = await this.refreshAccessToken();

                if (!accessToken) {
                    throw new Error('RefreshToken is NULL')
                }

                response = await fetch(`http://localhost:8080/updateProfilePhoto`, {
                    method: "POST",
                    headers: {
                        'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                    },
                    body: dataAuth,
                });
                console.log(response);
            }

            if (!response.ok) {
                throw new Error(`Ошибка: ${response.status}`);
            }

        } catch (error) {
            console.log(error)
            throw error;
        }
    }

    static async getProfileInfByNick(nickname) {
        try {
            let response = await fetch(`http://localhost:8080/getProfileByNickname?nickname=${nickname}`, {
                method: "Get",
                headers: {
                    'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                }
            });

            if (response.status === 401) {
                const accessToken = await this.refreshAccessToken();

                if (!accessToken) {
                    throw new Error('RefreshToken is NULL')
                }

                response = await fetch(`http://localhost:8080/getProfileByNickname?nickname=${nickname}`, {
                    method: "Get",
                    headers: {
                        "Authorization": `Bearer ${localStorage.getItem('accessToken')}`
                    }
                });
                console.log(response);
            }

            if (!response.ok) {
                throw new Error(`Ошибка: ${response.status}`);
            }

            return await response.json();
        } catch (error) {
            console.log(error)
            throw error;
        }
    }

    static async getProfilePhotoByNickname(nickname ) {
        try {
            let response = await fetch(`http://localhost:8080/getProfilePhoto?nickname=${nickname}`, {
                method: "Get",
                headers: {
                    'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                }
            });

            if (response.status === 401) {
                const accessToken = await this.refreshAccessToken();

                if (!accessToken) {
                    throw new Error('RefreshToken is NULL')
                }

                response = await fetch(`http://localhost:8080/getProfilePhoto?nickname=${nickname}`, {
                    method: "Get",
                    credentials: 'include',
                    headers: {
                        'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                    }
                });
                console.log(response);
            }

            if (!response.ok) {
                throw new Error(`Ошибка: ${response.status}`);
            }

            return await response.blob();
        } catch (error) {
            console.log(error)
            throw error;
        }
    }

    static async refreshAccessToken(dataAuth) {
        try {
            const refrehsResponse = await fetch("http://localhost:8080/getNewToken", {
                method: "Get",
                credentials: 'include'
            });

            if (!refrehsResponse.ok) {
                throw new Error("Не удалось обновить AccessToken");
            }

            const refreshData = await refrehsResponse.json();

            localStorage.setItem("accessToken", refreshData.accessToken);
            return refreshData.accessToken;
        } catch (error) {
            console.log(error)
            throw error;
        }
    }

    static async getDate() {
        try {
            console.log("send get request")
            let response = await fetch(`http://localhost:8081/getId`, {
                method: "Get",
                headers: {
                    'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                }
            });

            if (response.status === 401){ console.log('устарел access токен')}

            if (response.status === 401) {
                const accessToken = await this.refreshAccessToken();

                if (!accessToken) {
                    throw new Error('RefreshToken is NULL')
                }

                response = await fetch(`http://localhost:8081/getId`, {
                    method: "Get",
                    headers: {
                        'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                    }
                });
            }

            console.log(response);

            if (!response.ok) {
                throw new Error(`Ошибка: ${response.status}`);
            }

            return await response.text();
        } catch (error) {
            console.log(error)
            throw error;
        }
    }

    static async addNewPost(dataPost) {
        console.log("addNewPost is lounch")
        try {
            let response = await fetch(`http://localhost:8081/addNewPost`, {
                method: "POST",
                headers: {
                    'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                },
                body: dataPost
            });

            if (response.status === 401) {
                const accessToken = await this.refreshAccessToken();

                if (!accessToken) {
                    throw new Error('RefreshToken is NULL')
                }

                response = await fetch(`http://localhost:8081/addNewPost`, {
                    method: "POST",
                    headers: {
                        'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                    },
                    body: dataPost
                });
            }

            if (!response.ok) {
                throw new Error(`Ошибка: ${response.status}`);
            }

            console.log(response)
        } catch (error) {
            console.log(error)
            throw error;
        }
    }

    static async postLike(post_id){
        console.log(`postLike is lounch with post_id = ${post_id}`)
        try {
            let response = await fetch(`http://localhost:8081/likeClicked?post_id=${post_id}`, {
                method: "POST",
                headers: {
                    'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                }
            });

            if (response.status === 401) {
                const accessToken = await this.refreshAccessToken();

                if (!accessToken) {
                    throw new Error('RefreshToken is NULL')
                }

                response = await fetch(`http://localhost:8081/likeClicked?post_id=${post_id}`, {
                    method: "POST",
                    headers: {
                        'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                    }
                });
            }

            if (!response.ok) {
                throw new Error(`Ошибка: ${response.status}`);
            }

            return true;
        } catch (error) {
            console.log(error)
            throw error;
        }
    }

    static async getPostsList(limit, offset) {
        try {
            console.log("send get request")
            let response = await fetch(`http://localhost:8081/getPostsList?LIMIT=${limit}&OFFSET=${offset}`, {
                method: "Get",
                headers: {
                    'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                }
            });
            
            if (response.status === 401) {
                const accessToken = await this.refreshAccessToken();

                if (!accessToken) {
                    throw new Error('RefreshToken is NULL')
                }

                response = await fetch(`http://localhost:8081/getPostsList?LIMIT=${limit}&OFFSET=${offset}`, {
                    method: "Get",
                    headers: {
                        'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                    }
                });
            }

            console.log(response);

            if (!response.ok) {
                throw new Error(`Ошибка: ${response.status}`);
            }

            return await response.json();
        } catch (error) {
            console.log(error)
            throw error;
        }
    }

    static async getPostPhoto(id){
        try{
            let response = await fetch(`http://localhost:8081/getPostPhoto?photo_id=${id}`, {
                method: "GET",
                headers: {
                    'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                }
            });
            
            if (response.status === 401) {
                const accessToken = await this.refreshAccessToken();

                if (!accessToken) {
                    throw new Error('RefreshToken is NULL')
                }

                response = await fetch(`http://localhost:8081/getPostPhoto?photo_id=${id}`, {
                    method: "GET",
                    headers: {
                        'Authorization': `Bearer ${localStorage.getItem('accessToken')}`
                    }
                });
            }
            console.log(response);

            if (!response.ok) {
                throw new Error(`Ошибка: ${response.status}`);
            }

            return await response.blob();
        } catch (error) {
            console.log(error)
            throw error;
        }
    }
}