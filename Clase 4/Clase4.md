# 📘 Ejemplo Base de Sistema de Control con RFID, Servo, y Sensores

Este código implementa un sistema de control utilizando varios componentes, como RFID, un servo para una talanquera, sensores PIR e IR, un botón de chequeo, y una pantalla LCD. A continuación se explican las diferentes secciones del código y las funciones que realiza cada una.

## 🧰 Librerías y Definición de Pines

```cpp

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#define RST_PIN  9 // Pin de reset del RC522
#define SS_PIN   10 // Pin de selección del RC522
#define PIR_PIN  2 // Pin del sensor PIR
#define BUTTON_PIN 3 // Pin del botón de chequeo
#define SERVO_PIN 8 // Pin del servo de la talanquera
#define IR_PIN  4 // Pin del sensor IR para salida

```

### 🔍 Explicación
- **Librerías**: Se incluyen las librerías necesarias para controlar el lector RFID (MFRC522), un servo, una pantalla LCD I2C, y la EEPROM.
- **Definición de Pines**: Aquí se definen los pines utilizados por los diferentes componentes.

## 🛠️ Declaración de Objetos y Variables

```cpp

MFRC522 mfrc522(SS_PIN, RST_PIN); // RFID
Servo talanquera; // Servo
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD

volatile bool enableRFID = false; // Activación del RFID
volatile bool esSeguro = false; // Salida segura
volatile bool talanqueraState = false; // Estado de la talanquera
volatile bool isEntry = false; // Verificar si es entrada o salida

const int fotoPin = A0; // Pin para el sensor de luz
unsigned long previousMillis = 0;
const long interval = 500;

#define EEPROM_SIZE 512 // Tamaño de la EEPROM
#define UID_LENGTH 4 // Longitud del UID

```

### 🔍 Explicación
- **Objetos**: Se crean los objetos `mfrc522`, `talanquera`, y `lcd` para manejar el RFID, el servo, y la pantalla LCD respectivamente.
- **Variables**: Variables globales que controlan el estado del sistema, como si el RFID está activado, si es seguro realizar una operación, y el estado de la talanquera.
- **EEPROM**: Definición del tamaño de la EEPROM y la longitud del UID de las tarjetas RFID.

## 🚀 `setup()`

```cpp

void setup() {
  Serial.begin(9600);

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

  pinMode(PIR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(IR_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(PIR_PIN), detectMotion, RISING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), detectButton, FALLING);
  
  Serial.println("Sistema iniciado. Esperando movimiento...");
}

```

### 🔍 Explicación
- **Inicialización**: Configura la comunicación serial, inicia el lector RFID, el servo, y la pantalla LCD.
- **Pantalla LCD**: Muestra un mensaje inicial y luego lo borra después de 2 segundos.
- **Configuración de Pines**: Configura los pines del sensor PIR, el botón y el sensor IR.
- **Interrupciones**: Se configuran interrupciones para el sensor PIR y el botón para detectar eventos y activar funciones.

## 🔄 `loop()`

```cpp

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

```

### 🔍 Explicación
- **Lectura del Sensor de Luz**: Muestra en la LCD el valor de luminosidad detectado por el sensor.
- **Manejo de RFID**: Si el RFID está habilitado (`enableRFID` es verdadero), se procesa la lectura del RFID y se controla la talanquera para permitir la entrada o salida según corresponda.

## 👁️ Función `detectMotion()`

```cpp

void detectMotion() {
  enableRFID = true;
  Serial.println("Movimiento detectado, RFID habilitado.");
  lcd.setCursor(0, 1);
  lcd.print("RFID habilitado");
}

```

### 🔍 Explicación
- **Detectar Movimiento**: Esta función se activa cuando el sensor PIR detecta movimiento. Habilita el RFID y muestra un mensaje en la LCD.

## 🔄 Función `moverTalanquera()`

```cpp

void moverTalanquera() {
  talanquera.write(talanqueraState == 0 ? 90 : 0);
  talanqueraState = !talanqueraState; 
}

```

### 🔍 Explicación
- **Control del Servo**: Mueve la talanquera a 90 grados si está en 0, o la vuelve a 0 si está en 90 grados, alternando el estado.

## 📡 Función `processRFID()`

```cpp

void processRFID() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  printUID(mfrc522);
  MFRC522::Uid uid = mfrc522.uid;

  if (estaUID_EEPROM(uid)) {
    Serial.println("El UID ya está registrado en la EEPROM.");
  } else {
    guardarUID_EEPROM(uid);
    Serial.println("Nuevo UID guardado en la EEPROM.");
  }

  mfrc522.PICC_HaltA();
}

```

### 🔍 Explicación
- **Lectura de Tarjeta RFID**: Verifica si hay una nueva tarjeta RFID presente. Si la tarjeta no está registrada en la EEPROM, la guarda; de lo contrario, indica que ya está registrada.

## 🔘 Función `detectButton()`

```cpp

void detectButton() {
  isEntry = true;
  esSeguro = true;
  Serial.println("Modo: Entrada activado por botón.");
}

```

### 🔍 Explicación
- **Detección del Botón**: Se activa cuando el botón de chequeo es presionado. Indica que se está en modo de entrada y marca como seguro realizar la operación.

## 💾 Funciones para la EEPROM

### `guardarUID_EEPROM()`

```cpp

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

```

### 🔍 Explicación
- **Guardar UID en EEPROM**: Busca un espacio vacío en la EEPROM y guarda el UID de la tarjeta RFID en él.

### `estaUID_EEPROM()`

```cpp

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

```

### 🔍 Explicación
- **Verificar si UID está en EEPROM**: Compara el UID leído con los UIDs almacenados en la EEPROM para ver si ya está registrado.

### `eliminarUID_EEPROM()`

```cpp

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

```

### 🔍 Explicación
- **Eliminar UID de EEPROM**: Busca y elimina un UID específico de la EEPROM.

## 🖨️ `printUID()`

```cpp

void printUID(MFRC522 mfrc522) {
    Serial.print("UID: ");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();
}

```


