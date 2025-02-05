#define SENSOR_PIN 2  // Pin digital conectado al DO del módulo fotorresistencia
#define LED_PIN 13    // LED indicador

volatile bool luzDetectada = false;  // Variable compartida entre la interrupción y el loop

// Función de interrupción
void detectarCambioLuz() {
    luzDetectada = digitalRead(SENSOR_PIN) == LOW;  // Si el sensor activa LOW, es baja luz
}

void setup() {
    pinMode(SENSOR_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

    // Configurar la interrupción en el pin 2, activada por cualquier cambio (LOW a HIGH o HIGH a LOW)
    attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), detectarCambioLuz, CHANGE);

    Serial.begin(9600);
}

void loop() {
    if (luzDetectada) {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("Luz baja detectada, LED encendido");
    } else {
        digitalWrite(LED_PIN, LOW);
        Serial.println("Luz normal, LED apagado");
    }
    
    delay(100); // Pequeña pausa para estabilidad
}
