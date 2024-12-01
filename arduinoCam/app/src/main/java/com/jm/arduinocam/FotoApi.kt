package com.jm.arduinocam

import com.google.gson.annotations.SerializedName

data class FotoApi(
    @SerializedName(value = "foto")
    val foto : String = ""
)
