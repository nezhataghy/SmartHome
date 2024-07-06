const mongoose = require('mongoose');

const sensorReadingSchema = new mongoose.Schema({
  temperature: { 
    type: Number, 
    required: true 
  },
  humidity: {
    type: Number, 
    required: true 
  },
  flameValue: { 
    type: Number, 
    required: true 
  },
  user: { type: mongoose.Schema.Types.ObjectId, ref: 'User', required: true },
}, { timestamps: true });

module.exports = mongoose.model('SensorsReading', sensorReadingSchema);
