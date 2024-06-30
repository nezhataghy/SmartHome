const express = require('express');
const router = express.Router();
const ActuatorsController = require('../controllers/ActuatorsController');

router.get('/', ActuatorsController.getActuators);
router.post('/', ActuatorsController.createActuator);
router.get('/:id', ActuatorsController.getActuatorById);
router.put('/:id', ActuatorsController.updateActuator);
router.delete('/:id', ActuatorsController.deleteActuator);

module.exports = router;
