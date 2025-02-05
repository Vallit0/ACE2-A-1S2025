import cv2
import mediapipe as mp
import serial
import time

# Configurar conexión serial con Arduino (cambiar COM3 según tu puerto)
arduino = serial.Serial('COM3', 9600, timeout=1)
time.sleep(2)  # Esperar a que Arduino inicie

# Inicializar MediaPipe para detección de manos
mp_hands = mp.solutions.hands
mp_drawing = mp.solutions.drawing_utils
hands = mp_hands.Hands(min_detection_confidence=0.7, min_tracking_confidence=0.7)

# Iniciar cámara
cap = cv2.VideoCapture(0)

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        continue

    # Convertir imagen a RGB para MediaPipe
    image = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    image.flags.writeable = False
    results = hands.process(image)

    # Convertir imagen de nuevo a BGR para OpenCV
    image.flags.writeable = True
    image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)

    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            # Obtener coordenadas de los dedos índice y pulgar
            index_finger_tip = hand_landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_TIP]
            thumb_tip = hand_landmarks.landmark[mp_hands.HandLandmark.THUMB_TIP]

            # Calcular la distancia entre el índice y el pulgar
            distancia = ((index_finger_tip.x - thumb_tip.x) ** 2 + (index_finger_tip.y - thumb_tip.y) ** 2) ** 0.5

            # Determinar si los dedos están tocándose
            if distancia < 0.05:  # Umbral para considerar que los dedos están tocando
                cv2.putText(image, "ENCENDER RELÉ", (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
                arduino.write(b'ON\n')  # Enviar señal a Arduino
            else:
                cv2.putText(image, "APAGAR RELÉ", (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
                arduino.write(b'OFF\n')  # Enviar señal a Arduino

            # Dibujar puntos de referencia de la mano
            mp_drawing.draw_landmarks(image, hand_landmarks, mp_hands.HAND_CONNECTIONS)

    # Mostrar imagen con OpenCV
    cv2.imshow('Detección de manos', image)

    # Salir con la tecla 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Cerrar todo al salir
cap.release()
cv2.destroyAllWindows()
arduino.close()
