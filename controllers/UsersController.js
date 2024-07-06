const User = require('../models/User');
const SensorsReading = require('../models/Sensor');
const Actuator = require('../models/Actuator');


exports.getUsers = async (req, res) => {
    try {
      const users = await User.find({})
      .populate('sensorsreadings')
      .populate('actuators');
      
      res.json(users);
    } catch (err) {
      console.error('Error fetching users:', err);
      res.status(500).send('Internal Server Error');
    }
  };

  exports.getUserById = async (req, res) => {
    try {
      const user = await User.findById(req.params.id)
        .populate('sensorsreadings')
        .populate('actuators');

      if (!user) return res.status(404).json({ message: 'User not found' });
      res.json(user);
    } catch (err) {
      res.status(500).json({ message: err.message });
    }
  };

  exports.updateUser = async (req, res) => {
    try {
      const user = await User.findByIdAndUpdate(req.params.id, req.body, { new: true });
      if (!user) return res.status(404).json({ message: 'User not found' });
      res.json(user);
    } catch (err) {
      res.status(400).json({ message: err.message });
    }
  };
  

exports.deleteUser = async (req, res) => {
    try {
      const user = await User.findByIdAndDelete(req.params.id);
      if (!user) return res.sendStatus(404);
  
      // Delete all sensor readings linked to the user
      await SensorsReading.deleteMany({ user: req.params.id });
  
      // Delete all actuators linked to the user
      await Actuator.deleteMany({ user: req.params.id });
  
      res.sendStatus(204);
    } catch (err) {
      console.error('Error deleting user and linked objects:', err);
      res.status(500).send('Internal Server Error');
    }
  };
