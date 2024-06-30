const User = require('../models/User');
const jwt = require('jsonwebtoken');
const bcrypt = require('bcrypt');


exports.login = async (req, res) => {
    try {
      const { identifier, password } = req.body; // identifier can be username or email
      const user = await User.findOne({ $or: [{ username: identifier }, { email: identifier }] });
  
      if (!user || !(await bcrypt.compare(password, user.password))) {
        return res.status(401).send('Invalid credentials');
      }
  
      const token = jwt.sign({ id: user._id, username: user.username, email: user.email }, 'secret_key');
      console.log('Generated Token:', token);
      res.json({ token });
    } catch (err) {
      console.log('Login error:', err);
      res.status(500).send('Server error');
    }
  };

  exports.register = async (req, res) => {
    try {
      const { name, username, email, password } = req.body;
  
      // Input validation
      if (!name || !username || !email || !password) {
        return res.status(400).send('All fields are required');
      }
  
      // Check if user with the same username or email already exists
      const existingUser = await User.findOne({ $or: [{ username }, { email }] });
      if (existingUser) {
        return res.status(400).send('Username or email already exists');
      }
  
      // Hash the password before saving
      const hashedPassword = await bcrypt.hash(password, 10);
  
      const newUser = new User({ name, username, email, password: hashedPassword });
      await newUser.save();
      res.sendStatus(201);
    } catch (err) {
      console.log('Register error:', err);
      res.status(500).send('Error registering user');
    }
};
