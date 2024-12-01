const { Schema, model } = require("mongoose");
const leccionSchema = new Schema(
    {
      movimiento: {
        type: Number,
        required: true,
        trim: true
      }
    },
    {
      timestamps: true,
    }
  );
  module.exports = model("Movimiento", leccionSchema);