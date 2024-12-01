package com.jm.arduinocam

import retrofit2.http.Body
import retrofit2.http.Headers
import retrofit2.http.POST

interface ApiService {
    @Headers(value = ["Content-type: application/json"])
    @POST("mascotas/foto/")
    suspend fun postFoto(
        @Body foto: FotoApi
    ): FotoApi
}