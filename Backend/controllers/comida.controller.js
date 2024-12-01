
const Comida = require("../models/comida");

const controller = {};

controller.save = async (req, res, next)=>{
    try {
        const { comida_consumida } = req.body;

        const food = new Comida();

        food["comida_consumida"] = comida_consumida;

       const foodSaved = await food.save();

       if(!foodSaved){
        return res.status(409).json({error: "Error creating lesson"});
       }

       return res.status(201).json(foodSaved);
    } catch (error) {
        next(error);
    }
}

controller.findAll = async (req, res, next) => {
    try {
       const food = await Comida.find();
       return res.status(200).json({food});
    } catch (error) {
        next(error);
    }
}

module.exports = controller;