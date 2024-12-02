
const Foto = require("../models/foto");

const controller = {};

controller.save = async (req, res, next)=>{
    try {
        const { foto } = req.body;

        const foto_nueva = new Foto();

        foto_nueva["foto"] = foto;

       const _fotoSaved = await foto_nueva.save();

       if(!_fotoSaved){
        return res.status(409).json({error: "Error creating foto"});
       }

       return res.status(201).json(_fotoSaved);
    } catch (error) {
        next(error);
    }
}

controller.findLatest = async (req, res, next) => {
    try {
       const foto = await Foto.find().sort({ createdAt: -1 }).limit(1);
       return res.status(200).json(foto);
    } catch (error) {
        next(error);
    }
}

/*
controller.findAll = async (req, res, next) => {
    try {
       const foto = await Foto.find();
       return res.status(200).json({foto});
    } catch (error) {
        next(error);
    }
}
*/

module.exports = controller;