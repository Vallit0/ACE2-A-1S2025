/*
ARQUITECTURA DE COMPUTADORES Y ENSAMBLADORES 2
Ejemplo base by @Sebastian Valle
*/

// Librerías para RFID RC522
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#define RST_PIN  9 // Pin RC522
#define SS_PIN   10 // Pin RC522
#define PIR_PIN  2 // Sensor PIR
#define BUTTON_PIN 3 // Botón de chequeo
#define SERVO_PIN 8 // Servo de la talanquera
#define IR_PIN  4 // Sensor IR para salida

// Declaración de objetos
MFRC522 mfrc522(SS_PIN, RST_PIN); // RFID
Servo talanquera; // Servo
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD

// Variables
volatile bool enableRFID = false; // Activación del RFID
volatile bool esSeguro = false; // Salida segura
volatile bool talanqueraState = false; // Estado de la talanquera
volatile bool isEntry = false; // Verificar si es entrada o salida

const int fotoPin = A0; // Pin para el sensor de luz
unsigned long previousMillis = 0;
const long interval = 500;

#define EEPROM_SIZE 512 // Tamaño de la EEPROM
#define UID_LENGTH 4 // Longitud del UID

void setup() {
  Serial.begin(9600);

  // Inicialización del RFID
  SPI.begin();
  mfrc522.PCD_Init();
  
  talanquera.attach(SERVO_PIN);
  talanquera.write(0);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("ACE2-A"); 
  lcd.setCursor(0, 1);
  lcd.print("Sebastian"); // CAMBIAR POR NUMERO DE GRUPO 
  delay(2000);
  lcd.clear();

  // Configuración de pines
  pinMode(PIR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(IR_PIN, INPUT);

  // Interrupciones
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), detectMotion, RISING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), detectButton, FALLING);
  
  Serial.println("Sistema iniciado. Esperando movimiento...");
}

void loop() {
  int fotovalue = analogRead(fotoPin);
  lcd.setCursor(0, 0);
  lcd.print("Lumenes: ");
  lcd.print(fotovalue);

  if (millis() - previousMillis >= interval) {
    previousMillis = millis();
    lcd.setCursor(0, 0);
    lcd.print("Lumenes: ");
    lcd.print(fotovalue);
  }

  if (enableRFID) {
    processRFID();

    if (isEntry && esSeguro) {
      talanquera.write(90);
      Serial.println("Entrada autorizada.");
      lcd.setCursor(0, 1);
      lcd.print("Entrada autorizada");
    } else {
      talanquera.write(90);
      Serial.println("Salida autorizada.");
      lcd.setCursor(0, 1);
      lcd.print("Salida autorizada");
    }
    
    delay(1000);
    
    talanquera.write(0);
    
    enableRFID = false;
    Serial.println("Esperando nuevo movimiento...");
  }
}

// Función para detectar movimiento (PIR)
void detectMotion() {
  enableRFID = true;
  Serial.println("Movimiento detectado, RFID habilitado.");
  lcd.setCursor(0, 1);
  lcd.print("RFID habilitado");
}

// Función para manejar la talanquera
void moverTalanquera() {
  talanquera.write(talanqueraState == 0 ? 90 : 0);
  talanqueraState = !talanqueraState; 
}

// Función para manejar la lectura del RFID
void processRFID() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  printUID(mfrc522);
  MFRC522::Uid uid = mfrc522.uid;

  if (estaUID_EEPROM(uid)) {
    Serial.println("El UID ya está registrado en la EEPROM.");
    /*
    
    Agregar a LCD 
    */
    // Se debe Activar alguna variable bool como entrada 
  } else {
    guardarUID_EEPROM(uid);
    Serial.println("Nuevo UID guardado en la EEPROM.");
    /*
    Agregar a LCD 
    */
  }

  mfrc522.PICC_HaltA();
}

// Función para detectar el botón de entrada
void detectButton() {
  isEntry = true;
  esSeguro = true;
  Serial.println("Modo: Entrada activado por botón.");
  /*
  Aqui se deberia hacer un IF 
  con la variable que activa el RFID o algo similar
  
  */
}

// Funciones para la EEPROM
void guardarUID_EEPROM(MFRC522::Uid uid) {
    int startAddress = 0;
    while (startAddress < EEPROM_SIZE - UID_LENGTH) {
        bool emptySpace = true;
        for (int i = 0; i < UID_LENGTH; i++) {
            if (EEPROM.read(startAddress + i) != 0xFF) {
                emptySpace = false;
                break;
            }
        }
        if (emptySpace) {
            for (int i = 0; i < UID_LENGTH; i++) {
                EEPROM.write(startAddress + i, uid.uidByte[i]);
            }
            Serial.println("UID guardado en EEPROM.");
            return;
        }
        startAddress += UID_LENGTH;
    }
    Serial.println("EEPROM llena. No se pudo guardar el UID.");
}

bool estaUID_EEPROM(MFRC522::Uid uid) {
    int startAddress = 0;
    while (startAddress < EEPROM_SIZE - UID_LENGTH) {
        bool match = true;
        for (int i = 0; i < UID_LENGTH; i++) {
            if (EEPROM.read(startAddress + i) != uid.uidByte[i]) {
                match = false;
                break;
            }
        }
        if (match) {
            Serial.println("UID encontrado en EEPROM.");
            return true;
        }
        startAddress += UID_LENGTH;
    }
    Serial.println("UID no encontrado en EEPROM.");
    return false;
}

void eliminarUID_EEPROM(MFRC522::Uid uid) {
    int startAddress = 0;
    while (startAddress < EEPROM_SIZE - UID_LENGTH) {
        bool match = true;
        for (int i = 0; i < UID_LENGTH; i++) {
            if (EEPROM.read(startAddress + i) != uid.uidByte[i]) {
                match = false;
                break;
            }
        }
        if (match) {
            for (int i = 0; i < UID_LENGTH; i++) {
                EEPROM.write(startAddress + i, 0xFF);
            }
            Serial.println("UID eliminado de EEPROM.");
            return;
        }
        startAddress += UID_LENGTH;
    }
    Serial.println("UID no encontrado en EEPROM.");
}

// Imprimir el UID leído
void printUID(MFRC522 mfrc522) {
    Serial.print("UID: ");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();
}
