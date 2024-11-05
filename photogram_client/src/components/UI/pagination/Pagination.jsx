import React from "react";

const Pagination = ({page, changePage, pagesArray}) => {
    return (
        <div className="page_wrapper">
            {pagesArray.map(p =>
                <span key={p} className={page === p ? 'page page__current' : 'page'}
                    onClick={() => changePage(p)}>{p}</span>
            )}
        </div>
    )
}