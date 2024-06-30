const mongoose = require('mongoose');

const actuatorSchema = new mongoose.Schema({
    name: {
        type: String,
        required: true,
        unique : true
    },
    status: {
        type: String,
        required: true
    },
    user: { type: mongoose.Schema.Types.ObjectId, ref: 'User' }
} , { timestamps: true });

module.exports = mongoose.model('Actuator', actuatorSchema);
