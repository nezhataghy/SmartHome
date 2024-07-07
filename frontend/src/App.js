import React from 'react';
import { Routes, Route } from 'react-router-dom';
import Authentication from './pages/Authentication';
import Dashboard from './pages/Dashboard';

const App = () => {
  return (
    <Routes>
      <Route path="/" element={<Authentication />} />
      <Route path="/dashboard" element={<Dashboard />} />
    </Routes>
  );
};

export default App;
