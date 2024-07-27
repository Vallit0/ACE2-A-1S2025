const int ldrPin = 2; // Pin digital donde está conectado el módulo digital LDR
const int buzzerPin = 8; // Pin digital donde está conectado el buzzer
const int threshold = HIGH; // Umbral para activar el buzzer, ajusta según sea necesario

void setup() {
  pinMode(ldrPin, INPUT); // Configura el pin del módulo digital LDR como entrada
  pinMode(buzzerPin, OUTPUT); // Configura el pin del buzzer como salida
  Serial.begin(9600); // Inicializa la comunicación serial
}

void loop() {
  int ldrValue = digitalRead(ldrPin); // Lee el valor digital del módulo LDR
  
  Serial.print("Valor del módulo LDR: ");
  Serial.println(ldrValue); // Muestra el valor en el monitor serial

  if (ldrValue == threshold) {
    tone(buzzerPin, 1000); // Enciende el buzzer con un tono de 1000 Hz
  } else {
    noTone(buzzerPin); // Apaga el buzzer
  }

  delay(1000); // Espera 1 segundo antes de la próxima lectura
}
