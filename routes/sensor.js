const express = require('express');
const router = express.Router();
const sensorsController = require('../controllers/SensorsController');

router.get('/', sensorsController.getSensorsReading);
router.post('/', sensorsController.createSensorsReading);
router.get('/:id', sensorsController.getSensorById);
router.put('/:id', sensorsController.updateSensor);
router.delete('/:id', sensorsController.deleteSensor);

module.exports = router;
