import { useMqtt } from "../hooks/UseMqtt";
import DashboardElement from "../components/DashboardElement";
import DashboardForm from "../components/DashboardPublishForm";
import { useEffect, useState } from "react";
let cont = 0;
let cont2 = 0;

function Dashboard() {
  // Obtener los mensajes de los topics suscritos
  const { messages, publish } = useMqtt();
  const [lastMessages, setLastMessages] = useState(
    {
      nivelAgua : "",
      movimiento : "",
      comida : ""
    }
  );
  const waterLevelTopic = "/monitor/nivelAgua";
  const movementTopic = "/monitor/movimiento";
  const comidaTopic = "/monitor/comida";
  const baseUrl = "http://localhost:3500/mascotas/comida/";
  
  useEffect(() => {    
    //console.log(lastMessages);
    if(lastMessages && messages[waterLevelTopic]){
      if(messages[waterLevelTopic] == "1024"){
        if(cont<=10){
          console.log(`Mensaje publicado en /monitor/nivelAgua: ${messages[waterLevelTopic]}`);
          cont++
          publish(waterLevelTopic, messages[waterLevelTopic]);
        }
        
      }else{
        cont=0
        publish(waterLevelTopic, messages[waterLevelTopic]);
      }
      
      //Se actualiza el ultimo mensaje recibido
      setLastMessages({
        ...lastMessages,
        nivelAgua: messages[waterLevelTopic]
      });

    }

    if(lastMessages && messages[movementTopic]){
      if(messages[movementTopic] == "1023"){
        if(cont2<=10){
          console.log(`Mensaje publicado en /monitor/movimiento: ${messages[movementTopic]}`);
          cont2++
          publish(movementTopic, messages[movementTopic]);
        }
      }else{
        cont2 = 0;
      }

      setLastMessages({
        ...lastMessages,
        movimiento: messages[movementTopic]
      });

    }

    if(lastMessages && messages[comidaTopic] && lastMessages.comida != messages[comidaTopic]){
      //Se actualiza el ultimo mensaje recibido
      setLastMessages({
        ...lastMessages,
        comida: messages[comidaTopic]
      });

      try {
        /*
        const response = await fetch(baseUrl, {
          method: "POST",
          body: JSON.stringify({ comida_consumida:  messages[comidaTopic]})
        });

        if(response.ok){
          console.log(Registro de comida consumida guardado: ${messages[comidaTopic]});
        }
        */
       console.log("Comida publicada");
       
      } catch (error) {
        console.log(error);
      }
    }
  }, [messages]);

  const [ foto, setFoto ] = useState("");

  const updateFoto = (_foto) => {
    setFoto(_foto);
  }

  const getFoto = async () => {
    const baseUrl = "http://localhost:3500/mascotas/foto/"
    try {
      const response = await fetch(baseUrl);

      if(response.ok){
        const photo = await response.json();
        console.log(photo);
        updateFoto(photo.foto);
      }
    } catch (error) {
      console.log(error);
    }
  }

  useEffect(() => {
    getFoto();
  }, []);

  return (
    <main className="min-h-[100dvh] h-full bg-slate-900 text-slate-100 flex flex-col justify-center items-center gap-6 p-4">
      <h1 className="font-bold text-xl">Monitor de mascotas</h1>
      <div className="w-full max-w-2xl flex flex-col gap-10 justify-center items-center">
        {/* 
          Formulario para publicar mensajes 
          <DashboardForm topic="/test/message" label="Message"/>
        */}

        {/* Elementos para mostrar los mensajes de los topics suscritos */}
        <div className="flex flex-wrap gap-6 w-full justify-center items-center">
          <DashboardElement title={"Nivel de agua"} value={messages["/monitor/nivelAgua"]} />
          <DashboardElement title={"Agua del tanque"} value={messages["/monitor/nivelBomba"]} />
          <DashboardElement title={"Movimiento"} value={messages["/monitor/movimiento"]} />
          <DashboardElement title={"Comida consumida"} value={messages["/monitor/comida"]} />
          <img src={`data:image/jpeg;base64, ${foto}`} alt="LatestFoto" className="max-w-80"/>
        </div>
      </div>
    </main>
  );
}

export default Dashboard; 