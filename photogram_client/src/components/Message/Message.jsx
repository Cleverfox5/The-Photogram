import React from "react";

const Message = ({user, time, image}) {
    return(
        <div className="container">
            <imge src={image} alt="Fetched from server" />

        </div>
    )
}

export default Message;