package com.jm.arduinocam.home

import android.Manifest
import android.content.Context
import android.os.Environment
import android.view.ViewGroup
import androidx.camera.core.ImageCapture
import androidx.camera.core.ImageCaptureException
import androidx.camera.view.LifecycleCameraController
import androidx.camera.view.PreviewView
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.FloatingActionButton
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.viewinterop.AndroidView
import androidx.core.content.ContextCompat
import androidx.lifecycle.Lifecycle
import androidx.lifecycle.LifecycleOwner
import com.google.accompanist.permissions.ExperimentalPermissionsApi
import com.google.accompanist.permissions.isGranted
import com.google.accompanist.permissions.rememberMultiplePermissionsState
import com.google.accompanist.permissions.rememberPermissionState
import com.jm.arduinocam.Data
import com.jm.arduinocam.FotoApi
import com.jm.arduinocam.FotoViewModel
import okhttp3.MediaType.Companion.toMediaTypeOrNull
import okhttp3.RequestBody
import org.eclipse.paho.client.mqttv3.MqttClient
import java.io.File
import java.util.concurrent.Executor
import kotlin.io.encoding.Base64
import kotlin.io.encoding.ExperimentalEncodingApi
import kotlin.math.log


@OptIn(ExperimentalPermissionsApi::class)
@Composable
fun HomeScreen(){
    val fotoViewModel=FotoViewModel()
    val serverString ="tcp://192.168.252.13:1883"
    val username="rickgar"
    val pwd="123456"

    val clientID=MqttClient.generateClientId()
    val context = LocalContext.current
    try {
        fotoViewModel.connect(context, serverString, clientID, username, pwd)

    }catch (e:Exception){
        println(e)
    }
    val cameraController = remember {
        LifecycleCameraController(context)
    }
    val lifecycle= LocalLifecycleOwner.current
val permissions = rememberMultiplePermissionsState(permissions = listOf(Manifest.permission.CAMERA,Manifest.permission.WRITE_EXTERNAL_STORAGE,Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WAKE_LOCK,Manifest.permission.ACCESS_NETWORK_STATE,Manifest.permission.READ_PHONE_STATE , Manifest.permission.INTERNET))
val directory = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES).absoluteFile
    LaunchedEffect(Unit) {
        permissions.launchMultiplePermissionRequest()
    }

    if (Data.isPressed.value){
        val _executor=ContextCompat.getMainExecutor(context)
        takePicture(cameraController, _executor, directory, context)
        Data.isPressed.value=false
    }

    Scaffold(modifier = Modifier.fillMaxSize(), floatingActionButton = {
        FloatingActionButton(onClick = {
            val executor=ContextCompat.getMainExecutor(context)
            fotoViewModel.suscribe(context,"/monitor/movimiento")
            takePicture(cameraController, executor, directory, context)
        }) {
            Text(text = "Camara!")
        }
    }){
        if (permissions.allPermissionsGranted){

            CameraComposable(modifier = Modifier.padding(it), cameraController, lifecycle)
        }else{
            Text(text= "Permiso denegado", modifier = Modifier.padding(it))
        }
    }

}

private fun takePicture(cameraController: LifecycleCameraController, executor: Executor, directory: File, context: Context){
    val file = File.createTempFile("imagentest", ".jpg", directory)
    val outputDirectory = ImageCapture.OutputFileOptions.Builder(file).build()
    cameraController.takePicture(outputDirectory, executor, object : ImageCapture.OnImageSavedCallback {
        override fun onImageSaved(outputFileResults: ImageCapture.OutputFileResults) {
            println(outputFileResults.savedUri)
            uploadImage(file)
        }

        override fun onError(exception: ImageCaptureException) {
            println("Error")
        }

    })
}
@OptIn(ExperimentalEncodingApi::class)
private fun uploadImage(file: File){
    val image = file.readBytes()
    val _image = Base64.Default.encode(image)
    val viewModel=FotoViewModel()
    viewModel.addFoto(FotoApi(_image))
}
@Composable
fun CameraComposable(modifier:Modifier=Modifier, cameraController: LifecycleCameraController, lifecycle: LifecycleOwner){

    cameraController.bindToLifecycle(lifecycle)
    AndroidView(modifier = modifier, factory = {context->
        val previewView= PreviewView(context).apply {
            layoutParams = ViewGroup.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT,
            )
        }
        previewView.controller = cameraController
        previewView
    })

}