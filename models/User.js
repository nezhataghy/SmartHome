const mongoose = require("mongoose")
const userSchema = mongoose.Schema(
    {
        name: {
            type: String,
            required: true, 
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
        sensorsreadings: [{ type: mongoose.Schema.Types.ObjectId, ref: 'SensorsReading' }],

        actuators: [{ type: mongoose.Schema.Types.ObjectId, ref: 'Actuator' }],
    },
    {
        timestamps: true
    }
)

module.exports = mongoose.model("User", userSchema)
