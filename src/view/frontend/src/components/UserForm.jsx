import React, { useState } from 'react'

const UserForm = ({ onAddUser }) => {
    const [name, setName] = useState('')
    const [email, setEmail] = useState('')
    const [formError, setFormError] = useState('')

    const validateEmail = (email) => {
        const re = /^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$/
        return re.test(email)
    }
    const handleSubmit = (e) => {
        e.preventDefault()
        // Form validation
        if (!name.trim() || !email.trim()) {
            setFormError('Please fill in all fields')
            return
        }

        if (!validateEmail(email)) {
            setFormError('Please enter a valid email address')
            return
        }
        // Clear any existing error
        setFormError('')
        // Call the parent handler
        onAddUser({ name, email })
        // Reset form
        setName('')
        setEmail('')
    }
    return (
        <div className="form-container">
            <h2 className="form-title">Add New User</h2>
            {formError && (
                <div className="error-message">
                    {formError}
                </div>
            )}
            <form onSubmit={handleSubmit}>
                <div className="form-group">
                    <label htmlFor="name" className="form-label">
                        Name
                    </label>
                    <input
                        type="text"
                        id="name"
                        className="form-input"
                        value={name}
                        onChange={(e) => setName(e.target.value)}
                        placeholder="Enter user name"
                    />
                </div>
                <div className="form-group">
                    <label htmlFor="email" className="form-label">
                        Email
                    </label>
                    <input
                        type="email"
                        id="email"
                        className="form-input"
                        value={email}
                        onChange={(e) => setEmail(e.target.value)}
                        placeholder="Enter email address"
                    />
                </div>
                <button type="submit" className="form-submit">
                    Add User
                </button>
            </form>
        </div>
    )
}

export default UserForm