#include <Servo.h>

const int ldrPin = 2; // Pin digital donde está conectado el módulo digital LDR
const int servoPin = 8; // Pin digital donde está conectado el servo motor
const int threshold = HIGH; // Umbral para activar el servo, ajusta según sea necesario

Servo myServo; // Crea un objeto Servo

void setup() {
  pinMode(ldrPin, INPUT); // Configura el pin del módulo digital LDR como entrada
  myServo.attach(servoPin); // Configura el pin del servo motor
  Serial.begin(9600); // Inicializa la comunicación serial
}

void loop() {
  int ldrValue = digitalRead(ldrPin); // Lee el valor digital del módulo LDR
  
  Serial.print("Valor del módulo LDR: ");
  Serial.println(ldrValue); // Muestra el valor en el monitor serial

  if (ldrValue == threshold) {
    myServo.write(90); // Mueve el servo a la posición de 90 grados
  } else {
    myServo.write(0); // Mueve el servo a la posición de 0 grados
  }

  delay(1000); // Espera 1 segundo antes de la próxima lectura
}
