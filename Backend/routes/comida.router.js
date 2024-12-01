const express = require("express");
const router = express.Router();

const comidaController = require("../controllers/comida.controller");
 
router.get("/", comidaController.findAll);

router.post("/", comidaController.save);

module.exports = router;