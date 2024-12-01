const { Schema, model } = require("mongoose");
const fotoSchema = new Schema(
    {
      foto: {
        type: String,
        required: true,
      }
    },
    {
      timestamps: true,
    }
  );
  module.exports = model("Foto", fotoSchema);