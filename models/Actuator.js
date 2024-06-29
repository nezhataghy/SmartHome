const mongoose = require('mongoose');

const actuatorSchema = new mongoose.Schema({
  type: String,
  status: String,
  user: { type: mongoose.Schema.Types.ObjectId, ref: 'User' }
});

module.exports = mongoose.model('Actuator', actuatorSchema);
