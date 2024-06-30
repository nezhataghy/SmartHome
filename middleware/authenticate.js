const jwt = require('jsonwebtoken');

module.exports = async (req, res, next) => {
  try {
    const authHeader = req.headers.authorization;

    if (!authHeader) {
      console.log('No Authorization header');
      return res.status(401).send('Unauthorized');
    }

    const token = authHeader.split(' ')[1];

    if (!token) {
      console.log('No token found');
      return res.status(401).send('Unauthorized');
    }

    const decoded = await new Promise((resolve, reject) => {
      jwt.verify(token, 'secret_key', (err, decoded) => {
        if (err) {
          return reject(err);
        }
        resolve(decoded);
      });
    });

    console.log('Token verified, decoded user:', decoded);
    req.user = decoded;
    next();
  } catch (err) {
    console.log('Token verification failed:', err);
    return res.status(401).send('Unauthorized');
  }
};
