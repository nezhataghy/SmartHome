const express = require("express")
const bodyParser = require('body-parser');
require('dotenv').config();
const mongoose = require('mongoose');
const authenticate = require('./middleware/authenticate');
const wss = require('./websocketServer');

const app = express()

mongoose.connect(process.env.MONGO_URI)
.then(() => {
    console.log("Connected to the DB");
})
.catch((err) => {
    console.log({message: err.message})
})

// Middleware to parse incoming request bodies
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());

// Routes
app.use('/users', require('./routes/user'));
app.use('/auth', require('./routes/auth'));
app.use('/users', require('./routes/user'));
app.use('/sensors', authenticate, require('./routes/sensor'));
app.use('/actuators', authenticate, require('./routes/actuator'));

app.get("/", (req, res) => {
    res.send("SmartHome Project!")
});

app.listen(5000, () => {
    console.log('Server is running on port 5000');
});
