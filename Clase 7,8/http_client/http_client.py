import serial
import time
# Hacer requests a una API 
import requests

# Configuración del puerto serial
ser = serial.Serial('COM4', 9600, timeout=1)  # Cambia el puerto según sea necesario
time.sleep(2)  # Esperar a que la conexión serial esté estable

# URL de la API Flask
API_URL = "http://127.0.0.1:5000/verify_user"  # Ajusta la URL según donde esté corriendo tu API

def main():
    try:
        while True:
            # Leer los datos enviados desde el Arduino
            if ser.in_waiting > 0:
                incoming_data = ser.readline().decode('utf-8').strip()
                print(f"Data recibida desde el arduino: {incoming_data}")

                # Verifica si los datos del RFID han sido enviados
                if incoming_data.startswith("tag:"):
                    user_id = incoming_data.split(":")[1]  # Extraer el ID del usuario
                    print(f"Verificando el ID del usuario: {user_id}")

                    # Consumir la API para verificar el ID del usuario
                    user_id = user_id.strip()  # Eliminar espacios en blanco
                    response = requests.post(API_URL, json={"rfid": user_id})

                    if response.status_code == 200:
                        # Si el ID del usuario es válido, enviar el comando MOVE_SERVO
                        print("Usuario válido, moviendo el servo...")
                        ser.write(b"MOVE_SERVO\n")
                    elif response.status_code == 404:
                        print("Usuario no encontrado.")
                    else:
                        print(f"Error al consultar la API: {response.status_code}")
                        print(response.json())

                    time.sleep(1)  # Esperar un momento antes de la próxima lectura

            time.sleep(0.1)  # Evita sobrecargar el puerto serial con demasiadas lecturas

    except KeyboardInterrupt:
        print("Programa interrumpido manualmente.")
    finally:
        ser.close()  # Asegúrate de cerrar el puerto serial cuando el programa termine

if __name__ == "__main__":
    main()
