const express = require("express")
require('dotenv').config();
const mongoose = require('mongoose');


const app = express()






mongoose.connect(process.env.MONGO_URI)
.then(() => {
    console.log("Connected to the DB");
    app.listen(5000, () => {
        console.log('Server is running on port 5000');
    });
})
.catch((err) => {
    console.log({message: err.message})
})
