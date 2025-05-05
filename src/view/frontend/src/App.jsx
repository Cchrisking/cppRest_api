import { useState, useEffect } from 'react'
import './App.css'
import Header from './components/Header.jsx'
import Footer from "./components/Footer.jsx";
function App() {
    return (
        <div className="app-container">
            <Header/>
            <h1>Hello World</h1>
            {/* Gradually add components back one by one */}
            <Footer/>
        </div>
    )
}
export default App