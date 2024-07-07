import React, { useState, useEffect } from 'react';
import axios from 'axios';
import { Line } from 'react-chartjs-2';
import 'chart.js/auto';
import './Dashboard.css';
import logo from '../assets/images/icon.jpg';

const Dashboard = () => {
  const [sensors, setSensors] = useState([]);
  const [actuators, setActuators] = useState([]);

  useEffect(() => {
    const fetchData = async () => {
      try {
        const sensorResponse = await axios.get('http://52.87.216.189:5000/sensors', {
          headers: { Authorization: `Bearer ${localStorage.getItem('token')}` },
        });
        setSensors(sensorResponse.data);

        const actuatorResponse = await axios.get('http://52.87.216.189:5000/actuators', {
          headers: { Authorization: `Bearer ${localStorage.getItem('token')}` },
        });
        setActuators(actuatorResponse.data);
      } catch (error) {
        console.error('Error fetching data', error);
      }
    };

    fetchData();

    const intervalId = setInterval(fetchData, 5000); // Fetch data every 5 seconds

    return () => clearInterval(intervalId); // Cleanup interval on component unmount
  }, []);

  const handleActuatorToggle = async (id, currentState) => {
    try {
      let newState = currentState;

      if (currentState === 'ON') {
        newState = 'OFF';
      } else if (currentState === 'OFF') {
        newState = 'ON';
      } else if (currentState === 'LOCKED') {
        newState = 'UNLOCKED';
      } else if (currentState === 'UNLOCKED') {
        newState = 'LOCKED';
      }

      await axios.put(`http://52.87.216.189:5000/actuators/${id}`, { status: newState }, {
        headers: { Authorization: `Bearer ${localStorage.getItem('token')}` },
      });
      // Update actuator state locally
      setActuators(actuators.map(actuator => actuator._id === id ? { ...actuator, status: newState } : actuator));
    } catch (error) {
      console.error('Error updating actuator', error);
    }
  };

  const handleLogout = () => {
    localStorage.removeItem('token');
    window.location.href = '/';
  };

  const getData = (key) => {
    const labels = sensors.map((sensor, index) => index % 5 === 0 ? new Date(sensor.createdAt).toLocaleTimeString() : '');
    const data = sensors.map(sensor => sensor[key]);

    return {
      labels,
      datasets: [{
        label: key.charAt(0).toUpperCase() + key.slice(1), // Capitalize the first letter of the key
        data,
        fill: false,
        backgroundColor: 'rgba(54, 162, 235, 0.2)', // Blue color for graph points
        borderColor: 'rgba(54, 162, 235, 1)',
      }],
    };
  };

  const chartOptions = (title) => ({
    plugins: {
      title: {
        display: true,
        text: title,
      },
      legend: {
        display: false,
      },
    },
  });

  return (
    <div className="dashboard-container">
      <div className="top-bar">
        <img src={logo} alt="Logo" className="logo" />
        <button className="logout-button" onClick={handleLogout}>Logout</button>
      </div>
      <h1>Dashboard</h1>
      <div className="sensor-data">
        <div className="chart-container larger-chart">
          <Line data={getData('temperature')} options={chartOptions('Temperature')} />
        </div>
        <div className="chart-container larger-chart">
          <Line data={getData('humidity')} options={chartOptions('Humidity')} />
        </div>
        <div className="chart-container larger-chart">
          <Line data={getData('flameValue')} options={chartOptions('Flame Value')} />
        </div>
      </div>
      <div className="actuator-control">
        <h2>Actuator Control</h2>
        <div className="actuators">
          {actuators.map(actuator => (
            <div key={actuator._id} className="actuator">
              <div className="actuator-info">
                {actuator.name}: {actuator.status}
              </div>
              <button
                className={
                  actuator.status === 'ON' || actuator.status === 'UNLOCKED'
                    ? 'button-off'
                    : 'button-on'
                }
                onClick={() => handleActuatorToggle(actuator._id, actuator.status)}
              >
                {actuator.status === 'ON'
                  ? 'Turn OFF'
                  : actuator.status === 'OFF'
                  ? 'Turn ON'
                  : actuator.status === 'LOCKED'
                  ? 'UNLOCK'
                  : 'LOCK'}
              </button>
            </div>
          ))}
        </div>
      </div>
    </div>
  );
};

export default Dashboard;
