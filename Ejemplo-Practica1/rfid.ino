#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9    
#define SS_PIN 10    

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

void setup() {
    Serial.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init();
    
    // Llave predeterminada (FFFFFFFFFFFF)
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    Serial.println("Acerque una tarjeta para escribir...");
}

void loop() {
    if (!mfrc522.PICC_IsNewCardPresent()) return;
    if (!mfrc522.PICC_ReadCardSerial()) return;

    Serial.println("Tarjeta detectada!");

    escribirDatosEnTarjeta("Hola RFID!"); // Datos a escribir

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}

void escribirDatosEnTarjeta(String mensaje) {
    byte dataBlock[16]; 
    mensaje.getBytes(dataBlock, sizeof(dataBlock)); // Convertir String a array de bytes
    
    for (byte sector = 0; sector < 16; sector++) {
        for (byte bloque = 0; bloque < 3; bloque++) {  // Solo bloques de datos (evita bloques de control)
            byte bloqueReal = (sector * 4) + bloque;

            Serial.print("Escribiendo en bloque ");
            Serial.println(bloqueReal);

            MFRC522::StatusCode status = mfrc522.PCD_Authenticate(
                MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloqueReal, &key, &(mfrc522.uid));
            
            if (status != MFRC522::STATUS_OK) {
                Serial.println("Error en autenticación, bloque omitido.");
                continue;
            }

            status = mfrc522.MIFARE_Write(bloqueReal, dataBlock, 16);
            if (status == MFRC522::STATUS_OK) {
                Serial.println("Escritura exitosa!");
            } else {
                Serial.println("Error al escribir.");
            }
        }
    }
}
