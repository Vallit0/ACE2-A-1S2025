import paho.mqtt.client as mqtt
import serial

# Configuraciones del puerto serie
serial_port = 'COM7'  # Puerto en Windows
baud_rate = 9600

# Configuraciones del MQTT
mqtt_broker = 'broker.hivemq.com'
mqtt_port = 1883
mqtt_topic = 'clasesita'

# Configuración del puerto serie
ser = serial.Serial(serial_port, baud_rate, timeout=1)

# Umbral de distancia para mover el motor
movement_threshold = 10  # Umbral para activar el movimiento del motor

# Función para decidir si mover el motor basado en el número recibido
def decide_movement(number):
    if number > movement_threshold:
        # Mover el motor (esto es solo un ejemplo, ajusta según tu necesidad)
        ser.write(b'1')  # Enviar un comando para mover el motor
        print(f"Motor movido con número: {number}")
    else:
        # No mover el motor
        ser.write(b'1')  # Enviar un comando para detener el motor
        print(f"Motor detenido con número: {number}")

# Callback cuando el cliente recibe un mensaje
def on_message(client, userdata, msg):
    try:
        # Decodificar el mensaje recibido
        number = float(msg.payload.decode())
        print(f"Número recibido: {number}")

        # Decidir si mover el motor
        decide_movement(number)

    except ValueError:
        print("Mensaje recibido no es un número válido")

# Configuración del cliente MQTT
client = mqtt.Client()
client.on_message = on_message

# Conectar al broker MQTT
client.connect(mqtt_broker, mqtt_port, 60)

# Suscribirse al topic
client.subscribe(mqtt_topic)

# Iniciar el bucle del cliente MQTT
client.loop_forever()
