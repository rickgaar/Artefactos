const express = require("express");
const router = express.Router();

const fotoController = require("../controllers/foto.controller");
 
router.get("/", fotoController.findAll);

router.post("/", fotoController.save);

module.exports = router;