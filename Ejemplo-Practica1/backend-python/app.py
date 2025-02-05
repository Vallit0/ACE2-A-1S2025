from flask import Flask, jsonify, request
import serial
import threading

# Configuración de Flask
app = Flask(__name__)

# Variables globales
ultimo_dato = "."
ecg = 0
foto = 0.0
rfid = False
serial_port = None

# 📌 Cambia esto al puerto COM correcto de Windows (Ej: 'COM3', 'COM4', etc.)
PUERTO_COM = "COM3"  # ⚠️ Asegúrate de cambiar esto al puerto correcto en tu PC

# Detectar y conectar el puerto serial
def detectar_y_conectar_puerto():
    global serial_port
    try:
        serial_port = serial.Serial(PUERTO_COM, 9600, timeout=1)  # Ajusta el puerto en Windows
        print(f"✅ Conectado al puerto: {serial_port.port}")
    except serial.SerialException as e:
        print(f"❌ No se pudo abrir el puerto serial: {e}")

# Leer datos en tiempo real desde Arduino
def leer_datos_desde_arduino():
    global ultimo_dato, ecg, foto, rfid
    if serial_port is None:
        print("⚠ No hay un puerto serial abierto.")
        return
    
    print("✅ Iniciando lectura de datos desde Arduino...")

    while True:
        try:
            dato_recibido = serial_port.readline().decode('utf-8').strip()
            if dato_recibido:
                print(f"📥 [DEBUG] Dato recibido: {dato_recibido}")
                ultimo_dato = dato_recibido
                desempaquetar_datos_arduino()
        except Exception as e:
            print(f"❌ Error leyendo datos: {e}")

# Desempaquetar los datos recibidos
def desempaquetar_datos_arduino():
    global ecg, foto, rfid, ultimo_dato

    if not ultimo_dato or ultimo_dato == ".":
        print("⚠ [DEBUG] Dato vacío o inválido recibido. Ignorando...")
        return

    sensores = ultimo_dato.split()
    if len(sensores) < 3:
        print(f"❌ [ERROR] Formato incorrecto. Se esperaban 3 valores, pero llegaron: {len(sensores)}")
        return

    try:
        ecg = int(sensores[0])
        foto = float(sensores[1])
        rfid = sensores[2] != "0"
        print(f"✅ Datos desempaquetados -> ECG: {ecg}, Foto: {foto}, RFID: {rfid}")
    except ValueError as e:
        print(f"❌ [ERROR] Error al convertir datos: {e}")

# API - Endpoints

@app.route('/status', methods=['GET'])
def get_status():
    return "El servidor está funcionando correctamente 🚀"

@app.route('/ecg', methods=['GET'])
def get_datos_paciente():
    print("[SOLICITUD] Datos del paciente enviados.")
    return str(ecg)

@app.route('/get-datos-sensores', methods=['GET'])
def get_datos_sensores():
    print("[SOLICITUD] Datos de los sensores enviados.")
    datos_sensores = {
        "ecg": ecg,
        "foto": foto,
        "rfid": rfid
    }
    return jsonify(datos_sensores)

@app.route('/encender', methods=['POST'])
def enviar_datos_a_arduino():
    comando = request.form.get('comando', '').upper()
    
    if comando not in ["ON", "OFF"]:
        return "⚠ Comando no válido. Usa 'ON' o 'OFF'."

    if serial_port and serial_port.is_open:
        try:
            serial_port.write(f"{comando}\n".encode('utf-8'))
            print(f"📤 Mensaje enviado a Arduino: {comando}")
            return f"✅ Mensaje enviado a Arduino: {comando}"
        except Exception as e:
            return f"❌ Error enviando datos: {e}"
    else:
        return "⚠ El puerto serial no está abierto."

# Iniciar el servidor Flask y la lectura del puerto serial
if __name__ == '__main__':
    detectar_y_conectar_puerto()
    hilo_lectura = threading.Thread(target=leer_datos_desde_arduino, daemon=True)
    hilo_lectura.start()
    app.run(host='0.0.0.0', port=5000, debug=True)
