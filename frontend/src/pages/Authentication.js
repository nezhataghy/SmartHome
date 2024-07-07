import React, { useState } from 'react';
import axios from 'axios';
import { useNavigate } from 'react-router-dom';
import './Authentication.css';
import logo from '../assets/images/icon.jpg';

const Authentication = () => {
  const [loginData, setLoginData] = useState({ identifier: '', password: '' });
  const [registerData, setRegisterData] = useState({ name: '', username: '', email: '', password: '' });
  const [loginError, setLoginError] = useState('');
  const [registerError, setRegisterError] = useState('');
  const [successMessage, setSuccessMessage] = useState('');
  const navigate = useNavigate();

  const handleLoginChange = (e) => {
    const { name, value } = e.target;
    setLoginData({ ...loginData, [name]: value });
  };

  const handleRegisterChange = (e) => {
    const { name, value } = e.target;
    setRegisterData({ ...registerData, [name]: value });
  };

  const handleLoginSubmit = async (e) => {
    e.preventDefault();
    setLoginError('');
    setSuccessMessage('');
    try {
      const response = await axios.post('http://52.87.216.189:5000/auth/login', loginData);
      localStorage.setItem('token', response.data.token);
      setSuccessMessage('Login successful!');
      navigate('/dashboard');
    } catch (error) {
      setLoginError('Invalid credentials or server error');
      console.error('Login error:', error);
    }
  };

  const handleRegisterSubmit = async (e) => {
    e.preventDefault();
    setRegisterError('');
    setSuccessMessage('');
    try {
      await axios.post('http://52.87.216.189:5000/auth/register', registerData);
      setSuccessMessage('Registration successful!');
    } catch (error) {
      setRegisterError('Error registering user');
      console.error('Registration error:', error);
    }
  };

  return (
    <div className="auth-container">
      <div className="header">
        <img src={logo} alt="Logo" className="logo" />
        <h1 className="title">Welcome to SmartHome Control System</h1>
        <div className="spacer"></div>
      </div>
      <div className="actions">
        <div className="auth-section">
          <h2>Login</h2>
          {loginError && <p className="error">{loginError}</p>}
          {successMessage && <p className="success">{successMessage}</p>}
          <form onSubmit={handleLoginSubmit}>
            <input
              type="text"
              name="identifier"
              value={loginData.identifier}
              onChange={handleLoginChange}
              placeholder="Username or Email"
              required
            />
            <input
              type="password"
              name="password"
              value={loginData.password}
              onChange={handleLoginChange}
              placeholder="Password"
              required
            />
            <button type="submit">Login</button>
          </form>
        </div>
        <div className="auth-section">
          <h2>Register</h2>
          {registerError && <p className="error">{registerError}</p>}
          {successMessage && <p className="success">{successMessage}</p>}
          <form onSubmit={handleRegisterSubmit}>
            <input
              type="text"
              name="name"
              value={registerData.name}
              onChange={handleRegisterChange}
              placeholder="Name"
              required
            />
            <input
              type="text"
              name="username"
              value={registerData.username}
              onChange={handleRegisterChange}
              placeholder="Username"
              required
            />
            <input
              type="email"
              name="email"
              value={registerData.email}
              onChange={handleRegisterChange}
              placeholder="Email"
              required
            />
            <input
              type="password"
              name="password"
              value={registerData.password}
              onChange={handleRegisterChange}
              placeholder="Password"
              required
            />
            <button type="submit">Register</button>
          </form>
        </div>
      </div>
      <div className="footer">
        <p className="footer-text">By Nezha Aicha Taghy</p>
        <div className="social-links">
          <a href="https://github.com/nezhataghy" target="_blank" rel="noopener noreferrer">
            <img src="./images/github.png" alt="GitHub" className="social-icon" />
          </a>
          <a href="https://linkedin.com/in/nezha-aicha-taghi/" target="_blank" rel="noopener noreferrer">
            <img src="./images/linkedin.png" alt="LinkedIn" className="social-icon" />
          </a>
          <a href="https://twitter.com/ayshoush11" target="_blank" rel="noopener noreferrer">
            <img src="./images/x.png" alt="Twitter" className="social-icon" />
          </a>
        </div>
      </div>
    </div>
  );
};

export default Authentication;
