const mongoose = require("mongoose")
const userSchema = mongoose.Schema(
    {
        name: {
            type: String,
            required: true
        },
        username: {
            type: String, 
            required: true, 
            unique: true 
        },
        email: {
            type: String,
            required: true,
            unique: true 
        },
        password: {
            type: String,
            required: true
        },
        sensors: [{ 
            type: mongoose.Schema.Types.ObjectId, 
            ref: 'Sensor' }],
            actuators: [{
            type: mongoose.Schema.Types.ObjectId, 
            ref: 'Actuator' }],
    },
    {
        timestamps: true
    }
)

module.exports = mongoose.model("User", userSchema)
