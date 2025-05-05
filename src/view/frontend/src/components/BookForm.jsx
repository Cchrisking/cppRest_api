import React, { useState } from 'react'

const BookForm = ({ onAddBook }) => {
    const [title, setTitle] = useState('')
    const [author, setAuthor] = useState('')
    const [formError, setFormError] = useState('')

    const handleSubmit = (e) => {
        e.preventDefault()

        // Form validation
        if (!title.trim() || !author.trim()) {
            setFormError('Please fill in all fields')
            return
        }

        // Clear any existing error
        setFormError('')

        // Call the parent handler
        onAddBook({ title, author })

        // Reset form
        setTitle('')
        setAuthor('')
    }

    return (
        <div className="form-container">
            <h2 className="form-title">Add New Book</h2>

            {formError && (
                <div className="error-message">
                    {formError}
                </div>
            )}

            <form onSubmit={handleSubmit}>
                <div className="form-group">
                    <label htmlFor="title" className="form-label">
                        Title
                    </label>
                    <input
                        type="text"
                        id="title"
                        className="form-input"
                        value={title}
                        onChange={(e) => setTitle(e.target.value)}
                        placeholder="Enter book title"
                    />
                </div>

                <div className="form-group">
                    <label htmlFor="author" className="form-label">
                        Author
                    </label>
                    <input
                        type="text"
                        id="author"
                        className="form-input"
                        value={author}
                        onChange={(e) => setAuthor(e.target.value)}
                        placeholder="Enter author name"
                    />
                </div>

                <button type="submit" className="form-submit">
                    Add Book
                </button>
            </form>
        </div>
    )
}

export default BookForm