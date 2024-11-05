import { Navigate, Route, Routes } from "react-router-dom"
import { privateRoutes, publicRoutes } from "../../router"
import { useContext } from "react";
import { AuthContext } from "../../context";
import Loading from "./loading/Loading";


const Router = () => {
    const {isAuth, isLoading} = useContext(AuthContext);

    if (isLoading){
        return <Loading/>
    }
    return (
        isAuth
        ?
        <Routes>
            {privateRoutes.map(route =>
                <Route
                    exact={route.exact}
                    path={route.path}
                    element={<route.component />}
                />
            )}
            <Route path="authorization" element={<Navigate to="/messenger" />} />
            <Route path="*" element={<Navigate to="/error" />} />
        </Routes>
        :
        <Routes>
            {publicRoutes.map(route =>
                <Route
                    exact={route.exact}
                    path={route.path}
                    element={<route.component />}
                />
            )}
            <Route path="*" element={<Navigate to="/authorization" />} />
        </Routes>
    )
}

export default Router;