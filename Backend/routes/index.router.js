const express = require("express");
const router = express.Router();

const movementRouter = require("./movimiento.router");
const foodRouter = require("./comida.router");
const fotoRouter = require("./foto.router")
router.use("/comida", foodRouter);
router.use("/movimiento", movementRouter);
router.use("/foto", fotoRouter);

module.exports = router;