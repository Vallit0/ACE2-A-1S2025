const int mq135Pin = A0; // Pin analógico donde está conectado el sensor MQ135
const int buzzerPin = 8; // Pin digital donde está conectado el buzzer
const int threshold = 400; // Umbral para activar el buzzer, ajusta según sea necesario

void setup() {
  pinMode(buzzerPin, OUTPUT); // Configura el pin del buzzer como salida
  Serial.begin(9600); // Inicializa la comunicación serial
}

void loop() {
  int mq135Value = analogRead(mq135Pin); // Lee el valor del sensor
  
  Serial.print("Valor del sensor MQ135: ");
  Serial.println(mq135Value); // Muestra el valor en el monitor serial

  if (mq135Value > threshold) {
    tone(buzzerPin, 1000); // Enciende el buzzer con un tono de 1000 Hz
  } else {
    noTone(buzzerPin); // Apaga el buzzer
  }

  delay(1000); // Espera 1 segundo antes de la próxima lectura
}
