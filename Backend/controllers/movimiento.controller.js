
const Movimiento = require("../models/movimiento");

const controller = {};

controller.save = async (req, res, next)=>{
    try {
        const { movimiento } = req.body;

        const movement = new Movimiento();

        movement["movimiento"]=movimiento;

       const movementSaved = await movement.save();

       if(!movementSaved){
        return res.status(409).json({error: "Error creating lesson"});
       }

       return res.status(201).json(movementSaved);
    } catch (error) {
        next(error);
    }
}

controller.findAll = async (req, res, next) => {
    try {
       const movimiento = await Movimiento.find();
       return res.status(200).json({movimiento});
    } catch (error) {
        next(error);
    }
}

module.exports = controller;