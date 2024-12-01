package com.jm.arduinocam

import android.content.Context
import android.util.Log
import android.widget.Toast
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import org.eclipse.paho.client.mqttv3.IMqttActionListener
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken
import org.eclipse.paho.client.mqttv3.IMqttToken
import org.eclipse.paho.client.mqttv3.MqttCallback
import org.eclipse.paho.client.mqttv3.MqttMessage
import kotlin.math.log

class FotoViewModel: ViewModel() {
    private val api =ApiClient.apiService
    private lateinit var mqttClient :MQTTClient;

    fun addFoto(foto: FotoApi){
        viewModelScope.launch(Dispatchers.IO){
            try {
                val response = api.postFoto(foto)

                Log.i("MainViewModel",response.toString())
            }catch (e:Exception){
                when(e){
                    is retrofit2.HttpException -> {
                        e.message?.let { Log.i("MainViewmodel", it) }
                    }
                    else -> {
                        Log.i("MainViewModel", e.toString())
                    }
                }
            }
        }
    }

    fun connect(context: Context, serverURI:String,clientId:String, username:String,pwd:String){
        mqttClient = MQTTClient(context,serverURI,clientId);
        mqttClient.connect(
            username,
            pwd,
            object : IMqttActionListener {
                override fun onSuccess(asyncActionToken: IMqttToken?) {
                    Log.d(this.javaClass.name, "Connection success")

                    Toast.makeText(context, "MQTT Connection success", Toast.LENGTH_SHORT).show()


                }

                override fun onFailure(asyncActionToken: IMqttToken?, exception: Throwable?) {
                    Log.d(this.javaClass.name, "Connection failure: ${exception.toString()}")

                    Toast.makeText(context, "MQTT Connection fails: ${exception.toString()}",
                        Toast.LENGTH_SHORT).show()
                }
            },
            object : MqttCallback {
                override fun messageArrived(topic: String?, message: MqttMessage?) {
                    val msg = "Receive message: ${message.toString()} from topic: $topic"
                    Log.d(this.javaClass.name, msg)

                    Toast.makeText(context, msg, Toast.LENGTH_SHORT).show()
                }

                override fun connectionLost(cause: Throwable?) {
                    Log.d(this.javaClass.name, "Connection lost ${cause.toString()}")
                }

                override fun deliveryComplete(token: IMqttDeliveryToken?) {
                    Log.d(this.javaClass.name, "Delivery complete")
                }
            })
        mqttClient.recieve_info_broker()
    }

    fun suscribe(context: Context, topic:String){
        mqttClient.subscribe(
            topic,
            1,
            object : IMqttActionListener {
                override fun onSuccess(asyncActionToken: IMqttToken?) {
                    val msg = "Subscribed to: $topic"
                    Log.d(this.javaClass.name, msg)

                    Toast.makeText(context, msg, Toast.LENGTH_LONG).show()
                }

                override fun onFailure(asyncActionToken: IMqttToken?, exception: Throwable?) {
                    Log.d(this.javaClass.name, "Failed to subscribe: $topic")
                }
            })
    }
    fun disconnect(context: Context){
        mqttClient.disconnect(
            object : IMqttActionListener {
                override fun onSuccess(asyncActionToken: IMqttToken?) {
                    Log.d(this.javaClass.name, "Disconnected")

                    Toast.makeText(context, "MQTT Disconnection success", Toast.LENGTH_SHORT).show()
                }

                override fun onFailure(asyncActionToken: IMqttToken?, exception: Throwable?) {
                    Log.d(this.javaClass.name, "Failed to disconnect")
                }
            })
    }

}