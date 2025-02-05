#define RELE_PIN 7  // Conectamos el relé al pin 7
#define ECG_PIN A0  // Conectar la salida del AD8232 a A0
#define LO_PLUS 10  // Pin LO+ para verificar estado de electrodos
#define LO_MINUS 11 // Pin LO-


int ecg = 0; 
float foto = 10.4; 
int rfid = 0; 

void setup() {
    Serial.begin(9600);  // Inicia la comunicación serial
    pinMode(RELE_PIN, OUTPUT);  // Configura el relé como salida
    digitalWrite(RELE_PIN, LOW);  // Apaga el relé al inicio (foco apagado)
    //Serial.println("Arduino listo para recibir comandos...");    
    pinMode(LO_PLUS, INPUT);
    pinMode(LO_MINUS, INPUT);
    //Serial.println("ECG listo para medir señales...");
}

void loop() {
    // Mandamos Datos Random --
    // ECG FOTORESISTENCIA RFID  
    Serial.println("10 0.78 0");
    // Mandamos Datos del ECG 
    if ((digitalRead(LO_PLUS) == 1) || (digitalRead(LO_MINUS) == 1)) {
        ecg = 0; 
    } else {
        ecg = analogRead(ECG_PIN);  // Leer la señal del sensor
         // Enviar datos al puerto Serial
    }
    String mensaje = String(ecg) + " " + String(foto) + " " + String(rfid);
    Serial.println(mensaje);
}
