const Actuator = require('../models/Actuator');
const User = require('../models/User');
const WebSocket = require('ws'); 

const wss = new WebSocket.Server({ port: 8080 }); 

wss.on('connection', (ws) => {
  console.log('Client connected');

  ws.on('message', (message) => {
    console.log('Received:', message);
  });

  ws.on('close', () => {
    console.log('Client disconnected');
  });
});

exports.getActuators = async (req, res) => {
  try {
    const actuators = await Actuator.find({ user: req.user.id });
    res.json(actuators);
  } catch (err) {
    res.status(500).send(err);
  }
};

exports.createActuator = async (req, res) => {
  try {
    // Check if an actuator with the same name already exists
    const existingActuator = await Actuator.findOne({ name: req.body.name, user: req.user.id });
    if (existingActuator) {
      return res.status(400).json({ error: 'An actuator with this name already exists' });
    }

    const newActuator = new Actuator({ ...req.body, user: req.user.id });
    await newActuator.save();

    // Update user document to add this actuator
    await User.findByIdAndUpdate(req.user.id, {
      $push: { actuators: newActuator._id }
    });

    res.sendStatus(201);
  } catch (err) {
    console.error('Error saving actuator:', err);
    res.status(500).json({ error: 'Failed to save actuator' });
  }
};

exports.getActuatorById = async (req, res) => {
  try {
    console.log(`Received ID: ${req.params.id}`);
    
    const actuator = await Actuator.findById(req.params.id);
    if (!actuator) {
      console.log('Actuator not found');
      return res.sendStatus(404);
    }

    res.json(actuator);
  } catch (err) {
    console.error('Error:', err);
    res.status(500).send(err);
  }
};


exports.updateActuator = async (req, res) => {
  try {
    const actuator = await Actuator.findByIdAndUpdate(req.params.id, req.body, { new: true });
    if (!actuator) return res.sendStatus(404);

  // Broadcast status update
    wss.clients.forEach((client) => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(JSON.stringify(actuator));
    }
  });

    res.json(actuator);
  } catch (err) {
    console.error('Error updating actuator:', err);
    res.status(500).send({ message: 'Internal server error', error: err.message });
  }
};

exports.deleteActuator = async (req, res) => {
  try {
    // Find and delete the actuator
    const actuator = await Actuator.findByIdAndDelete(req.params.id);
    if (!actuator) return res.sendStatus(404);

    // Remove the actuator reference from the user's actuators array
    await User.findByIdAndUpdate(actuator.user, {
      $pull: { actuators: actuator._id }
    });

    res.sendStatus(204);
  } catch (err) {
    console.error('Error deleting actuator:', err);
    res.status(500).send(err);
  }
};
