import { useState, useEffect } from 'react'
import './App.css'
import 'bootstrap/dist/css/bootstrap.min.css';
import Header from './components/Header.jsx'
import Footer from "./components/Footer.jsx";
import Menu from './components/Menu.jsx'
import BookList from './components/BookList.jsx'
import UserList from './components/UserList.jsx'
import BookForm from './components/BookForm.jsx'
import UserForm from './components/UserForm.jsx'
import api from "./axios.js";
function App() {
    const [activeSection, setActiveSection] = useState('books')
    const [books, setBooks] = useState([])
    const [users, setUsers] = useState([])
    const [loading, setLoading] = useState(false)
    const [error, setError] = useState(null)

    // API Base URLs
    const BOOK_API_URL = 'http://localhost:8080/books'
    const USER_API_URL = 'http://localhost:9080/users'

    // Fetch books from API
    const fetchBooks = async () => {
        setLoading(true)
        try {
            const response = await api.libraryApi.get('/books')
            setUsers(response.data);
            setError(null);
        } catch (err) {
            console.error('Error fetching users:', err)
            setError('Failed to load users. Please try again later.')
        } finally {
            setLoading(false)
        }
    };
    // Fetch users from API
    const fetchUsers = async () => {
        setLoading(true)
        try {
            const response = await api.userApi.get('/users')
            setUsers(response.data);
            setError(null);
        } catch (err) {
            console.error('Error fetching users:', err)
            setError('Failed to load users. Please try again later.')
        } finally {
            setLoading(false)
        }
    };
// Add new book
    const addBook= async (book) => {
        try {
            console.log('Sending user data:', user);
            const response = await api.userApi().post('/users', book);
            console.log('User added successfully:', response.data);
            fetchBooks(); // Refresh the user list
        } catch (err) {
            console.error('Error details:', err.response || err);
            setError(`Failed to add user: ${err.message}`);
        }
    };
    // Delete book
    const deleteBook = async (id) => {
        try {
            await api.userApi().delete(`/books/${id}`);
            fetchBooks(); // Refresh the user list
        } catch (err) {
            console.error('Error deleting user:', err);
            setError('Failed to delete user. Please try again.');
        }
    };

    // Add new user
    const addUser = async (user) => {
        try {
            console.log('Sending user data:', user);
            const response = await api.userApi().post('/users', user);
            console.log('User added successfully:', response.data);
            fetchUsers(); // Refresh the user list
        } catch (err) {
            console.error('Error details:', err.response || err);
            setError(`Failed to add user: ${err.message}`);
        }
    };
    // Delete user
    const deleteUser = async (id) => {
        try {
            await api.userApi().delete(`/users/${id}`);
            fetchUsers(); // Refresh the user list
        } catch (err) {
            console.error('Error deleting user:', err);
            setError('Failed to delete user. Please try again.');
        }
    };

    // Load data based on active section
    useEffect(() => {
        if (activeSection === 'books') {
            fetchBooks()
        } else if (activeSection === 'users') {
            fetchUsers()
        }
    }, [activeSection])

    // Handle menu click
    const handleMenuClick = (section) => {
        setActiveSection(section)
    }

    return (
        <div className="app-container">
            <Header/>
            <Menu activeSection={activeSection} onMenuClick={handleMenuClick} />
            <main className="container col-6">
                <h1>Hello lib</h1>
                {error && <div className="error-message">{error}</div>}
                {loading ? (
                    <div className="loading">Loading...</div>
                ) : (
                    <>
                        {activeSection === 'books' && (
                            <div className="section-container">
                                <BookForm onAddBook={addBook} />
                                <BookList books={books} onDeleteBook={deleteBook} />
                            </div>
                        )}

                        {activeSection === 'users' && (
                            <div className="section-container">
                                <UserForm onAddUser={addUser} />
                                <UserList users={users} onDeleteUser={deleteUser} />
                            </div>
                        )}
                    </>
                )}
            </main>
            <Footer/>
        </div>
    )
}
export default App