import React, { useState, useEffect } from "react";
import { Line } from "react-chartjs-2";
import { Chart as ChartJS } from "chart.js/auto";
import { BASE_URL } from '.logic/constants.js';
export const App = () => {
  const [dataPaciente, setDataPaciente] = useState([]); // Datos del paciente
  const [labels, setLabels] = useState([]); // Tiempos
  const [sensorData, setSensorData] = useState({ foto: 0, ecg: 0, rfid: false });

  useEffect(() => {
    const fetchData = async () => {
      try {
        /* Obtener datos del ECG
         localhost:8080/ecg 
         si le pones localhost -> al correrlo en otro dispositivo no va a funcionar 
         browser se va a ir al localhost de ese dispositivo
         por ejemplo, tu celular no tiene nada corriendo en su localhost
         por lo que va a dar error, asi que te dejo los comandos para averiguar tu ip 
          ipconfig -> en windows
          ifconfig -> en linux
        
          te debes ir a la seccion Wireless LAN adapter Wi-Fi:
          y buscar la direccion IPv4 Address.
          esa es tu ip, asi que cambia localhost por tu ip

        Tambien debes levantar el servidor de este frontend con el comando 
        npm run dev -- --host 

        lo que resultara en -------------------
  ➜  Local:   http://localhost:5173/
  ➜  Network: http://<TU_IP>:<TU_PUERTO>/

  -> Esta IP la puedes sustituir en el archivo constants.js de la carpeta logic 


 ➜  Ojo que esto, probablemente lo debas cambiar para la calificacion. 
  En la calificacion, van a compartir datos desde su celular a 1 computador asi que 
  si lo estas probando desde tu red wi fi en tu casa, la IP de la red que genera tu celular 
  y la IP que genera tu red wi fi en tu casa son distintas.

  El Computador 1 -> va a tener el backend y el frontend 
  El Computador 2 -> se va a conectar desde el navegador a la ip del Computador 1

  Tu Celular -> Va a generar una Red. 

  Es muy probable que la IP Cambie por lo que les recomiendo testear o preguntarme. 
        */
        const response = await fetch(`${BASE_URL}/ecg`);
        const textData = await response.text();
        const datoPaciente = parseFloat(textData);

        // Obtener otros datos del sensor
        const respuesta = await fetch(`${BASE_URL}/get-datos-sensores`);
        const textData2 = await respuesta.text();
        const datosSensores = JSON.parse(textData2);

        console.log(datosSensores);
        setSensorData(datosSensores);
        
        if (!isNaN(datoPaciente)) {
          const timestamp = new Date().toLocaleTimeString();

          setDataPaciente((prevData) => [...prevData.slice(-9), datoPaciente]); // Guardar últimos 10 datos
          setLabels((prevLabels) => [...prevLabels.slice(-9), timestamp]); // Guardar últimos 10 timestamps
        }
      } catch (error) {
        console.error("Error al obtener datos del paciente:", error);
      }
    };

    fetchData();
    /* Actualizar datos cada 2 segundos */ 
    const interval = setInterval(fetchData, 2000);

    return () => clearInterval(interval);
  }, []);

  return (
    <div className="App">
      <div className="dataCard">
        <h2>Datos en tiempo real</h2>
        <p><strong>Foto:</strong> {sensorData.foto}</p>
        <p><strong>ECG:</strong> {sensorData.ecg}</p>
        <p><strong>RFID:</strong> {sensorData.rfid ? "Detectado" : "No Detectado"}</p>
      </div>

      <div className="dataCard">
        <Line
          data={{
            labels: labels,
            datasets: [
              {
                label: "Datos del Paciente",
                data: dataPaciente,
                borderColor: "#064FF0",
                backgroundColor: "rgba(6, 79, 240, 0.2)",
                borderWidth: 2,
                tension: 0.3,
              },
            ],
          }}
          options={{
            plugins: {
              title: {
                display: true,
                text: "Cardiaco del Paciente",
                font: { size: 18 },
              },
            },
            scales: {
              x: { title: { display: true, text: "Tiempo" } },
              y: { title: { display: true, text: "Valor" } },
            },
          }}
        />
      </div>
    </div>
  );
};