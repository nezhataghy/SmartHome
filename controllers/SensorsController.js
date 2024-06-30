const SensorsReading = require('../models/Sensor'); // Ensure this matches your actual model name and file path
const User = require('../models/User');

exports.getSensorsReading = async (req, res) => {
  try {
    const sensorsReading = await SensorsReading.find({ user: req.user.id });
    res.json(sensorsReading);
  } catch (err) {
    console.error('Error fetching sensor readings:', err);
    res.status(500).send('Internal Server Error');
  }
};

exports.createSensorsReading = async (req, res) => {
    try {
      const newSensorReading = new SensorsReading({
        temperature: req.body.temperature,
        humidity: req.body.humidity,
        flameValue: req.body.flameValue,
        user: req.user.id // Assuming req.user.id exists after authentication middleware
      });
      
      await newSensorReading.save();
  
      // Update user document to add this sensor reading
      await User.findByIdAndUpdate(req.user.id, {
        $push: { sensorsreadings: newSensorReading._id }
      });
  
      res.sendStatus(201); // Send HTTP status 201 (Created) if successful
    } catch (err) {
      console.error('Error saving sensor reading:', err);
      res.status(500).json({ error: 'Failed to save sensor reading' });
    }
  };

exports.getSensorById = async (req, res) => {
  try {
    const sensor = await SensorsReading.findById(req.params.id);
    if (!sensor) return res.sendStatus(404);
    res.json(sensor);
  } catch (err) {
    res.status(500).send('Internal Server Error');
  }
};

exports.updateSensor = async (req, res) => {
  try {
    const sensor = await SensorsReading.findByIdAndUpdate(req.params.id, req.body, { new: true });
    if (!sensor) return res.sendStatus(404);
    res.json(sensor);
  } catch (err) {
    res.status(500).send('Internal Server Error');
  }
};

exports.deleteSensor = async (req, res) => {
    try {
      // Find and delete the sensor reading
      const sensor = await SensorsReading.findByIdAndDelete(req.params.id);
      if (!sensor) return res.sendStatus(404);
  
      // Remove the sensor reference from the user's sensorsreadings array
      await User.findByIdAndUpdate(sensor.user, {
        $pull: { sensorsreadings: sensor._id }
      });
  
      res.sendStatus(204);
    } catch (err) {
      console.error('Error deleting sensor:', err);
      res.status(500).send('Internal Server Error');
    }
  };
