const express = require("express");
const router = express.Router();

const movimientoController = require("../controllers/movimiento.controller");
 
router.get("/", movimientoController.findAll);

router.post("/", movimientoController.save);

module.exports = router;