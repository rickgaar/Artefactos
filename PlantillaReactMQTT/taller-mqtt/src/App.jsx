import { MqttProvider } from "./context/MqttContext";
import Dashboard from "./pages/Dashboard";

function App() {
  const brokerUrl = "ws://127.0.0.1:9001"; // URL de tu broker MQTT -> ws://<IP>:<Puerto>
  const options = {
    username: "rickgar", // Usuario MQTT
    password: "123456", // Contraseña MQTT
  };

  const topics = ["/monitor/nivelAgua", "/monitor/nivelBomba", "/monitor/movimiento"]; // Lista de topics a los que se suscribirá

  return (
    <MqttProvider brokerUrl={brokerUrl} options={options} topics={topics}>
      <Dashboard />
    </MqttProvider>
  );
}

export default App;