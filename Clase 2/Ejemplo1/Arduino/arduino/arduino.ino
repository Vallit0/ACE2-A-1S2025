#include <Servo.h>

const int servoPin = 9;  // Pin para el servo
Servo myServo;

bool atZero = true;  // Variable para rastrear la posición del servo

void setup() {
  Serial.begin(9600);
  myServo.attach(servoPin);
  myServo.write(0);  // Inicialmente en 0 grados
}

void loop() {
  if (Serial.available()) {
    String commandStr = Serial.readStringUntil('\n');
    int command = commandStr.toInt();  // Convertir el comando recibido a entero

    if (command == 1) {
      if (atZero) {
        myServo.write(90);  // Mover el servo a 90 grados
        Serial.println("Servo movido a 90 grados");
        atZero = false;
      } else {
        myServo.write(0);   // Mover el servo a 0 grados
        Serial.println("Servo movido a 0 grados");
        atZero = true;
      }
    } else {
      Serial.println("Comando no válido. Envíe 1.");
    }
  }
}
