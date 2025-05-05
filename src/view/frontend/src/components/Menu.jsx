import React from 'react'

const Menu = ({ activeSection, onMenuClick }) => {
    const menuItems = [
        { id: 'books', label: 'Books' },
        { id: 'users', label: 'Users' }
    ]

    return (
        <nav className="nav-menu">
            <ul className="menu-items">
                {menuItems.map(item => (
                    <li key={item.id} className="menu-item">
                        <a
                            className={`menu-link ${activeSection === item.id ? 'active' : ''}`}
                            onClick={() => onMenuClick(item.id)}
                        >
                            {item.label}
                        </a>
                    </li>
                ))}
            </ul>
        </nav>
    )
}

export default Menu