const express = require('express');
const router = express.Router();
const sensorsController = require('../controllers/SensorsController');
const authenticate = require('../middleware/authenticate');

router.use(authenticate);

// Define the routes and map them to the controller functions
router.get('/', sensorsController.getSensorsReading);
router.post('/', sensorsController.createSensorsReading);
router.get('/:id', sensorsController.getSensorById);
router.put('/:id', sensorsController.updateSensor);
router.delete('/:id', sensorsController.deleteSensor);

module.exports = router;
