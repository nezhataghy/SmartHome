import React, { useState, useEffect } from 'react';
import axios from 'axios';
import { Line } from 'react-chartjs-2';
import 'chart.js/auto';
import './Dashboard.css';

const Dashboard = () => {
  const [sensors, setSensors] = useState([]);
  const [actuators, setActuators] = useState([]);

  useEffect(() => {
    const fetchData = async () => {
      try {
        const sensorResponse = await axios.get('http://52.87.216.189:5000/sensors', {
          headers: { Authorization: `Bearer ${localStorage.getItem('token')}` },
        });
        console.log('Sensor Data:', sensorResponse.data); // Log the entire sensor data
        if (sensorResponse.data.length > 0) {
          console.log('First Sensor Entry:', sensorResponse.data[0]); // Log the first sensor entry
        }
        setSensors(sensorResponse.data);

        const actuatorResponse = await axios.get('http://52.87.216.189:5000/actuators', {
          headers: { Authorization: `Bearer ${localStorage.getItem('token')}` },
        });
        console.log('Actuator Data:', actuatorResponse.data); // Log the entire actuator data
        if (actuatorResponse.data.length > 0) {
          console.log('First Actuator Entry:', actuatorResponse.data[0]); // Log the first actuator entry
        }
        setActuators(actuatorResponse.data);
      } catch (error) {
        console.error('Error fetching data', error);
      }
    };

    fetchData();
  }, []);

  const handleActuatorToggle = async (id, currentState) => {
    try {
      await axios.put(`http://52.87.216.189:5000/actuators/${id}`, { status: currentState === 'ON' ? 'OFF' : 'ON' }, {
        headers: { Authorization: `Bearer ${localStorage.getItem('token')}` },
      });
      // Update actuator state locally
      setActuators(actuators.map(actuator => actuator._id === id ? { ...actuator, status: currentState === 'ON' ? 'OFF' : 'ON' } : actuator));
    } catch (error) {
      console.error('Error updating actuator', error);
    }
  };

  const handleLogout = () => {
    localStorage.removeItem('token');
    // redirecting to auth page
    window.location.href = '/';
  };

  const getData = (key) => {
    const labels = sensors.map((sensor, index) => index % 5 === 0 ? new Date(sensor.createdAt).toLocaleTimeString() : '');
    const data = sensors.map(sensor => sensor[key]);

    return {
      labels,
      datasets: [{
        label: key,
        data,
        fill: false,
        backgroundColor: 'rgba(54, 162, 235, 0.2)', // Blue color for graph points
        borderColor: 'rgba(54, 162, 235, 1)',
      }],
    };
  };

  return (
    <div className="dashboard-container">
      <div className="top-bar">
        <button className="logout-button" onClick={handleLogout}>Logout</button>
      </div>
      <h1>Dashboard</h1>
      <div className="sensor-data">
        <div className="chart-container larger-chart">
          <Line data={getData('temperature')} options={{ plugins: { legend: { display: false } } }} />
        </div>
        <div className="chart-container larger-chart">
          <Line data={getData('humidity')} options={{ plugins: { legend: { display: false } } }} />
        </div>
        <div className="chart-container larger-chart">
          <Line data={getData('flameValue')} options={{ plugins: { legend: { display: false } } }} />
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
                className={actuator.status === 'ON' ? 'button-on' : 'button-off'}
                onClick={() => handleActuatorToggle(actuator._id, actuator.status)}
              >
                {actuator.status === 'ON' ? 'Turn OFF' : 'Turn ON'}
              </button>
            </div>
          ))}
        </div>
      </div>
    </div>
  );
};

export default Dashboard;
