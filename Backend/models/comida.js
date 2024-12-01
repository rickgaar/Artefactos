const { Schema, model } = require("mongoose");
const leccionSchema = new Schema(
    {
      comida_consumida: {
        type: Number,
        required: true,
        trim: true
      }
    },
    {
      timestamps: true,
    }
  );
  module.exports = model("Comida", leccionSchema);