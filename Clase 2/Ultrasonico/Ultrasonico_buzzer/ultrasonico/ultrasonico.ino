const int trigPin = 2; // Pin digital donde está conectado el pin Trig del sensor
const int echoPin = 3; // Pin digital donde está conectado el pin Echo del sensor
const int buzzerPin = 8; // Pin digital donde está conectado el buzzer
const int distanceThreshold = 10; // Umbral de distancia en centímetros para activar el buzzer

void setup() {
  pinMode(trigPin, OUTPUT); // Configura el pin Trig como salida
  pinMode(echoPin, INPUT);  // Configura el pin Echo como entrada
  pinMode(buzzerPin, OUTPUT); // Configura el pin del buzzer como salida
  Serial.begin(9600); // Inicializa la comunicación serial
}

void loop() {
  // Genera un pulso en el pin Trig
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Lee el pulso del pin Echo
  long duration = pulseIn(echoPin, HIGH);

  // Calcula la distancia en centímetros
  float distance = duration * 0.0344 / 2;

  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Compara la distancia con el umbral
  if (distance < distanceThreshold) {
    tone(buzzerPin, 1000); // Enciende el buzzer con un tono de 1000 Hz
  } else {
    noTone(buzzerPin); // Apaga el buzzer
  }

  delay(500); // Espera 0.5 segundos antes de la próxima lectura
}
