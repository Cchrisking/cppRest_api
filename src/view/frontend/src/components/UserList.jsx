import React from 'react'

const UserList = ({ users, onDeleteUser }) => {
    if (users.length === 0) {
        return (
            <div className="list-container">
                <h2 className="list-title">Users</h2>
                <div style={{ padding: '1.5rem', textAlign: 'center' }}>
                    No users found. Add a new user to get started.
                </div>
            </div>
        )
    }

    return (
        <div className="list-container">
            <h2 className="list-title">Users</h2>
            <table className="list-table">
                <thead>
                <tr>
                    <th>ID</th>
                    <th>Name</th>
                    <th>Email</th>
                    <th>Actions</th>
                </tr>
                </thead>
                <tbody>
                {users.map(user => (
                    <tr key={user.id}>
                        <td>{user.id}</td>
                        <td>{user.name}</td>
                        <td>{user.email}</td>
                        <td>
                            <button className="edit-btn">Edit</button>
                            <button
                                className="delete-btn"
                                onClick={() => onDeleteUser(user.id)}
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

export default UserList