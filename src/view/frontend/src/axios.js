// In a new file like src/api/axios.js
import axios from 'axios';

const userApi = axios.create({
    baseURL: 'http://localhost:9080',
    headers: {
        'Content-Type': 'application/json',
    },
    // Below are options that might help with CORS
    withCredentials: false,
});
const libraryApi = axios.create({
    baseURL: 'http://localhost:8080',
    headers: {
        'Content-Type': 'application/json',
    },
    // Below are options that might help with CORS
    withCredentials: false,
});

export default {userApi,libraryApi};