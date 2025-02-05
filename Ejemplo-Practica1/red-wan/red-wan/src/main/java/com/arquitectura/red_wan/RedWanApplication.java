package com.arquitectura.red_wan;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.web.bind.annotation.*;
import com.fazecast.jSerialComm.SerialPort;


// Importamos el Buffer y el InputBuffer
import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;


import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;
/* Programado por Sebastian Valle

NOTA> antes de inicializar el programa verificar
serialPort = puertosDisponibles[5];

El numero de puerto esta en el listado general.
 */


import java.util.ArrayList;
import java.util.List;

// CRUD - POST - PUT - DELETE - GET
// Frontend -> Backend
@SpringBootApplication
@RestController
// CORS-ORIGIN un programa trata de hacer requests a localhost
@CrossOrigin(origins = "*") // Lo vamos a ver despues
public class RedWanApplication {

	// En estas variables vamos a guardar todo
	private static SerialPort serialPort;
	private static String ultimoDato = ".";

	private static int ecg = 0;
	private static float foto = 0;
	private static boolean rfid = false;


	public static void main(String[] args) {
		// Spring 
		SpringApplication.run(RedWanApplication.class, args);
		// Detectamos el puerto
		detectarYConectarPuerto();  // Buscar y conectar automáticamente al primer puerto disponible

		// Sistemas Multitarea tenemos que inicializar un hilo
		// este hilo lo que va hacer -> Leer Datos Arduino
		// API     | Leyendo datos del arduino
		new Thread(RedWanApplication::leerDatosDesdeArduino).start(); // Iniciar hilo para leer datos en tiempo real
	}

	// Detectamos y conectamos al primer puerto COM disponible
	private static void detectarYConectarPuerto() {
		SerialPort[] puertosDisponibles = SerialPort.getCommPorts();

		if (puertosDisponibles.length == 0) {
			System.out.println("❌ No se encontraron puertos COM disponibles.");
			return;
		}

		System.out.println("🔍 Puertos disponibles:");
		for (int i = 0; i < puertosDisponibles.length; i++) {
			System.out.println("📌 [" + i + "] " + puertosDisponibles[i].getSystemPortName() + " - " + puertosDisponibles[i].getDescriptivePortName());
		}

		// Seleccionar el primer puerto disponible
		serialPort = puertosDisponibles[0];  // Antes estaba en el índice 5, ahora toma el primero disponible.

		// Configuración del puerto serial
		serialPort.setBaudRate(9600);
		serialPort.setNumDataBits(8);
		serialPort.setNumStopBits(1);
		serialPort.setParity(SerialPort.NO_PARITY);

		if (serialPort.openPort()) {
			System.out.println("✅ Conectado al puerto: " + serialPort.getSystemPortName());
		} else {
			System.out.println("❌ Error al abrir el puerto serial.");
		}
	}

	// Desempaquetar los datos
	private static void desempaquetarDatosArduino() {
	// "10 0.78 0 | 1"
		if (ultimoDato == null || ultimoDato.trim().isEmpty() || ultimoDato.equals(".")) {
			System.out.println("⚠ [DEBUG] Dato vacío o inválido recibido. Ignorando...");
			return;
		}

		// System.out.println("✅ [DEBUG] Procesando dato: " + ultimoDato);

		String[] sensores = ultimoDato.trim().split("\\s+"); // Divide por cualquier cantidad de espacios

		if (sensores.length < 3) {
			System.out.println("❌ [ERROR] Formato incorrecto. Se esperaban 3 valores, pero llegaron: " + sensores.length);
			return;
		}

		try {
			ecg = Integer.parseInt(sensores[0]);
			foto = Float.parseFloat(sensores[1]);
			rfid = !sensores[2].equals("0");

			// System.out.println("✅ Datos desempaquetados correctamente -> ECG: " + ecg + ", Foto: " + foto + ", RFID: " + rfid);
		} catch (NumberFormatException e) {
			System.out.println("❌ [ERROR] Error al convertir datos: " + e.getMessage());
		}
	}

	// Leer datos en tiempo real desde Arduino
	private static void leerDatosDesdeArduino() {
		if (serialPort == null || !serialPort.isOpen()) {
			System.out.println("⚠ No hay un puerto serial abierto.");
			return;
		}

		System.out.println("✅ Iniciando lectura de datos desde Arduino...");

		try {
			// variable = input(esta cosa)
			serialPort.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 0, 0);
			InputStream inputStream = serialPort.getInputStream();
			// input stream >> teclad

			BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));

			while (true) {
				String datoRecibido = reader.readLine();  // Leer una línea completa
				if (datoRecibido == null || datoRecibido.trim().isEmpty()) {
					System.out.println("⚠ [DEBUG] Se recibió una línea vacía. Ignorando...");
					continue;  // Evita procesar líneas vacías
				}

				if (!datoRecibido.isEmpty()) {
					//System.out.println("📥 [DEBUG] Dato recibido: " + datoRecibido);
					ultimoDato = datoRecibido;  // Guardar el último dato recibido
					//System.out.println(ultimoDato);
					//System.out.println("Llego hasta aqui");
					desempaquetarDatosArduino();

				}
			}
		} catch (Exception e) {
			System.out.println("❌ Error leyendo datos: " + e.getMessage());
		}
	}


	// GET - POST - PUT - DELETE
	// @

	@GetMapping("/status")
	public String getStatus() {
		System.out.println("[REQUEST ARQUITECTURA 2] REQUEST");
		return "El servidor está funcionando correctamente 🚀";
	}

	@PostMapping("/team")
	public String crearUsuario(@RequestBody Map<String, Object> datos) {
		String nombre = (String) datos.get("nombre");
		String lider = (String) datos.get("lider");

		System.out.println("[INGRESO DE EQUIPO] " + nombre + " " + lider);
		enviarDatosAArduino(nombre + "," + lider);

		return "Bienvenido a Arqui 2, " + nombre;
	}

	@GetMapping("/ecg")
	public String getDatosPaciente() {
		System.out.println("[SOLICITUD] Datos del paciente enviados.");
		return String.valueOf(ecg);  // Devuelve el último dato recibido desde Arduino
	}


	@GetMapping("/get-datos-sensores")
	public Map<String, Object> getDatosSensores() {
		System.out.println("[SOLICITUD] Datos del sensores enviados.");
		// No debemos devolver el ultimo dato sino un json, con los datos empaquetados
		// sensor 1 - Fotorresistencia
		// sensor 2 - ECG
		// sensor 3 - RFID
		Map<String, Object> datosSensores = new HashMap<>();
		datosSensores.put("ecg", ecg);
		datosSensores.put("foto", foto);
		datosSensores.put("rfid", rfid);

		// devolvemos un json con los datos
		// de los sensores
		// { "ecg": int, "foto": foto, "rfid": bool }
		return datosSensores;
	}

	// Tambien podemos enviar datos al arduino
	/*
	* NOTA> BODY->  x-www-form-urlencoded
	* Key> Comando, Value> ON
	* Key> Comando, Value> OFF
	* curl -X POST "http://192...:8080/encender" -d "comando=ON"
	* curl -X POST "http://localhost:8080/encender" -d "comando=ON"
	*
	* */
	@PostMapping("/encender")
	public String enviarDatosAArduino(@RequestParam String comando) {
		if (serialPort != null && serialPort.isOpen()) {
			try {
				// ✅ Validar el comando antes de enviarlo
				if (!comando.equalsIgnoreCase("ON") && !comando.equalsIgnoreCase("OFF")) {
					return "⚠ Comando no válido. Usa 'ON' o 'OFF'.";
				}

				OutputStream outputStream = serialPort.getOutputStream();
				String mensaje = comando + "\n";  // Asegurar que el mensaje se envía con salto de línea
				outputStream.write(mensaje.getBytes(StandardCharsets.UTF_8));
				outputStream.flush();
				System.out.println("📤 Mensaje enviado a Arduino: " + mensaje);
				return "✅ Mensaje enviado a Arduino: " + mensaje;
			} catch (Exception e) {
				e.printStackTrace();
				return "❌ Error enviando datos: " + e.getMessage();
			}
		} else {
			return "⚠ El puerto serial no está abierto.";
		}
	}


}
