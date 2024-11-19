import Authorization from "../pages/Authorization"
import ErrorPage from "../pages/ErrorPage"
import FriendSearch from "../pages/FriendSearch"
import Messenger from "../pages/Messenger"
import PostsPage from "../pages/PostsPage"
import Registration from "../pages/Registration"
import UserProfile from "../pages/UserProfile"

export const privateRoutes = [
    {path: '/profile/:nickname', component: UserProfile, exact: true},
    {path: '/messenger', component: Messenger, exact: true},
    {path: '/friendSearch', component: FriendSearch, exact: true},
    {path: '/', component: Messenger, exact: true},
    {path: '/error', component: ErrorPage, exact: true},
    {path: '/postsPage', component: PostsPage, exact: true}
]

export const publicRoutes = [
    {path: '/authorization', component: Authorization, exact: true},
    {path: '/registration', component: Registration, exact: true}
]