// In src/api/axios.js
import axios from 'axios';

const userApi = axios.create({
    baseURL: 'http://localhost:9080',
    headers: {
        'Content-Type': 'application/json',
    },
    withCredentials: false,
});

const libraryApi = axios.create({
    baseURL: 'http://localhost:8080',
    headers: {
        'Content-Type': 'application/json',
    },
    withCredentials: false,
});

// Export both instances as named exports
export default { userApi, libraryApi };