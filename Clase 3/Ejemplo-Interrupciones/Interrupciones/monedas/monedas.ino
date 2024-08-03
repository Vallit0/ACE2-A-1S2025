
/*
Arquitectura de Computadores y Ensambladores 2
Created by Sebastian Valle 
Notas: 
attachInterrupt() => cuando recibe una señal especificada entra en la ISR dada
interrupts()      => habilita las interrupciones (default)
noInterrupts()    => deshabilita las interrupciones

las variables que se usen adentro de las ISR, 
es mejor especificarlas como volatile. 

volatile int variable = algo; 
----------------------------------------------------------------
Idea de como hacer la practica: 
¿Que vamos a hacer en clase? [X]
1- Recopilar Fotorresistencia y ver en LCD (loop)             [X] 
2- Interrumpir al detectar presencia (ISR #1)                 [ ] 
3- Funcion con RFID (ISR #2)                                  [X] 
  3-1 Mover Servo-Motor                                       [X] 
  3-2 Esperar a que se quite                                  [ ]  
4- Interrumpir al detectar Moneda (ISR #3)                    [ ] 
5- EEPROM                                                     [ ]

----------------------------------------------------------------
*/


/*
 Board I2C / TWI pins
Uno       =   A4 (SDA), A5 (SCL)
Mega2560  =   20 (SDA), 21 (SCL)
Leonardo  =   2 (SDA), 3 (SCL)
Due       =   20 (SDA), 21 (SCL), SDA1, SCL1
 */
// Libreria LiquidCrystal
#include <LiquidCrystal_I2C.h> //DESCARGAR LIBRERÍA: https://github.com/ELECTROALL/Codigos-arduino/blob/master/LiquidCrystal_I2C.zip
LiquidCrystal_I2C lcd(0x27,16,2); // si no te sale con esta direccion  puedes usar (0x3f,16,2) || (0x27,16,2)  ||(0x20,16,2) 

// Libreria Servo 
#include <Servo.h> 
const int talanqueraPin = 8; 
Servo talanquera; 

// Estado de la talanquera 
bool talanqueraState = 0; 

// Estado Boton - Arduino 
const int buttonPin = 2; 
volatile bool presionado = false; 


void setup() {
  // Boton para la interrupcion 
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), moverTalanquera, CHANGE);
  // inicializacion LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ACE2-A"); 
  lcd.setCursor(0,1);
  lcd.print("Sebastian");
  delay(20000);
  lcd.clear(); 
  lcd.setCursor(0,0);
  // Test Servo
  lcd.print("Test Servo");
  talanquera.attach(talanqueraPin); 
  talanquera.write(0); 
  delay(1000); 
  talanquera.write(90);
}

// 

void loop() { 
  lcd.display();
  delay(500);
  lcd.noDisplay();
  delay(500);
}

// Interrupcion con Boton - Ejemplo 
void moverTalanquera() {
  Serial.println("pressed");
  talanquera.write((talanqueraState == 0 ) ? 90 : 0);
  talanqueraState = !talanqueraState; 
}

// Interrupcion con PIR 