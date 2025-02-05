import serial
import threading
import time
from flask import Flask, request, jsonify
from flask_cors import CORS

# Configuración del puerto serial (modifica según tu sistema)
PUERTO_SERIAL = "COM3"  # En Linux/Mac puede ser "/dev/ttyUSB0"
VELOCIDAD_BAUDIOS = 9600

# Intentar conectar con Arduino
try:
    arduino = serial.Serial(PUERTO_SERIAL, VELOCIDAD_BAUDIOS, timeout=1)
    time.sleep(2)  # Esperar a que Arduino inicie
    print(f"✅ Conectado a Arduino en {PUERTO_SERIAL}")
except serial.SerialException:
    print("❌ No se pudo conectar al puerto serial.")
    arduino = None

# Almacenar el último dato recibido de Arduino
ultimo_dato = "No hay datos aún."

# Iniciar aplicación Flask
app = Flask(__name__)
CORS(app)  # Habilitar CORS para evitar problemas en el frontend

# 🔍 Función para leer datos en segundo plano
def leer_datos_arduino():
    global ultimo_dato
    if arduino:
        while True:
            try:
                dato_recibido = arduino.readline().decode("utf-8").strip()
                if dato_recibido:
                    print(f"📥 [DEBUG] Dato recibido: {dato_recibido}")
                    ultimo_dato = dato_recibido
            except Exception as e:
                print(f"❌ Error leyendo datos: {e}")

# Iniciar un hilo para leer los datos en tiempo real
hilo_lectura = threading.Thread(target=leer_datos_arduino, daemon=True)
hilo_lectura.start()

# ✅ Endpoint para verificar si el servidor está funcionando
@app.route("/status", methods=["GET"])
def get_status():
    print("[REQUEST ARQUITECTURA 2] REQUEST")
    return jsonify({"status": "El servidor está funcionando correctamente 🚀"})

# ✅ Endpoint para obtener los datos del paciente desde Arduino
@app.route("/datos-paciente", methods=["GET"])
def get_datos_paciente():
    print("[SOLICITUD] Datos del paciente enviados.")
    return jsonify({"ultimo_dato": ultimo_dato})

# ✅ Endpoint para enviar datos a Arduino
@app.route("/encender", methods=["POST"])
def enviar_datos_a_arduino():
    if not arduino:
        return jsonify({"error": "⚠ El puerto serial no está abierto."}), 500

    comando = request.form.get("comando")
    
    if comando not in ["ON", "OFF"]:
        return jsonify({"error": "⚠ Comando no válido. Usa 'ON' o 'OFF'."}), 400

    try:
        mensaje = comando + "\n"
        arduino.write(mensaje.encode("utf-8"))
        arduino.flush()
        print(f"📤 Mensaje enviado a Arduino: {mensaje}")
        return jsonify({"mensaje": f"✅ Mensaje enviado a Arduino: {mensaje}"})
    except Exception as e:
        print(f"❌ Error enviando datos: {e}")
        return jsonify({"error": f"❌ Error enviando datos: {e}"}), 500

# ✅ Endpoint para registrar equipos (simula el endpoint `/team`)
@app.route("/team", methods=["POST"])
def registrar_equipo():
    datos = request.json  # Recibir JSON en el body
    nombre = datos.get("nombre", "Desconocido")
    lider = datos.get("lider", "Desconocido")

    print(f"[INGRESO DE EQUIPO] {nombre} - {lider}")
    return jsonify({"mensaje": f"Bienvenido a Arqui 2, {nombre}"})

# Iniciar servidor Flask
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8080, debug=True)
