import React from 'react'

const BookList = ({ books, onDeleteBook }) => {
    if (books.length === 0) {
        return (
            <div className="list-container">
                <h2 className="list-title">Books</h2>
                <div style={{ padding: '1.5rem', textAlign: 'center' }}>
                    No books found. Add a new book to get started.
                </div>
            </div>
        )
    }

    return (
        <div className="list-container">
            <h2 className="list-title">Books</h2>
            <table className="list-table">
                <thead>
                <tr>
                    <th>ID</th>
                    <th>Title</th>
                    <th>Author</th>
                    <th>Actions</th>
                </tr>
                </thead>
                <tbody>
                {books.map(book => (
                    <tr key={book.id}>
                        <td>{book.id}</td>
                        <td>{book.title}</td>
                        <td>{book.author}</td>
                        <td>
                            <button className="edit-btn">Edit</button>
                            <button
                                className="delete-btn"
                                onClick={() => onDeleteBook(book.id)}
                            >
                                Delete
                            </button>
                        </td>
                    </tr>
                ))}
                </tbody>
            </table>
        </div>
    )
}

export default BookList