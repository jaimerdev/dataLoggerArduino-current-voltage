#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);

#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;

#include <SPI.h>
#include <SD.h>

#define SSpin 10

File archivo;

float sensibilidadcc = 0.100;
float lectura;
float vin;
float vout;
float R1 = 100000.0;
float R2 = 10000.0;

void setup() {
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  Serial.begin(9600);
  pinMode(A0, INPUT);
  Serial.println("Inicializando tarjeta...");
  if(!SD.begin(SSpin)) {
    Serial.println("Fallo en inicialización!");
    lcd.init();
    lcd.backlight();
    lcd.setCursor(2, 1);
    lcd.print("ERROR AL GENERAR");
    lcd.setCursor(5, 2);
    lcd.print("EL ARCHIVO");
    delay(3000);
    return;
  }
  Serial.println("Inicialización correcta");

  lcd.init();
  lcd.backlight();
}

void loop() {

  archivo = SD.open("prueba6.txt", FILE_WRITE);
  lectura = analogRead(A1);
  vout = (lectura * 5.00) / 1024.0;
  vin = (vout / (R2 / (R1 + R2)));
  if(vin < 0.09) {
    vin = 0.0;
  }

  float Idc = calculoCorriente(500);
  float P = Idc * vin;
  
  if(archivo) {
    DateTime now = rtc.now();
    archivo.print(now.day(), DEC);
    archivo.print("/");
    archivo.print(now.month(), DEC);
    archivo.print("/");
    archivo.print(now.year(), DEC);
    archivo.print(";");
    archivo.print(now.hour(), DEC);
    archivo.print(":");
    archivo.print(now.minute(), DEC);
    archivo.print(":");
    archivo.print(now.second(), DEC);
    archivo.print(";");
    archivo.print(vin);
    archivo.print(";");
    archivo.println(Idc, 3);
    // archivo.print(";");
    // archivo.println(P, 3);

    Serial.print(now.day(), DEC);
    Serial.print("/");
    Serial.print(now.month(), DEC);
    Serial.print("/");
    Serial.print(now.year(), DEC);
    Serial.print(";");
    Serial.print(now.hour(), DEC);
    Serial.print(":");
    Serial.print(now.minute(), DEC);
    Serial.print(":");
    Serial.print(now.second(), DEC);
    Serial.print(";");
    Serial.print(vin);
    Serial.print(";");
    Serial.println(Idc, 3);
    // Serial.print(";");
    // Serial.println(P, 3);
    archivo.close();

    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print(now.hour(), DEC);
    lcd.setCursor(8, 0);
    lcd.print(":");
    lcd.setCursor(9, 0);
    lcd.print(now.minute(), DEC);
    lcd.setCursor(11, 0);
    lcd.print(":");
    lcd.setCursor(12, 0);
    lcd.print(now.second(), DEC);
    lcd.setCursor(0, 1);
    lcd.print("V = ");
    lcd.print(vin);
    lcd.print(" [V]");
    lcd.setCursor(0, 2);
    lcd.print("I = ");
    lcd.print(Idc, 3);
    lcd.print(" [A]");
    // lcd.setCursor(0, 3);
    // lcd.print("P = ");
    // lcd.print(P, 3);
    // lcd.print(" [W]");
  } else {
    Serial.println("Error al abrir el archivo");
  }
  delay(1000);
}

float calculoCorriente(int numeroMuestras) {
   float leerAcs712;
   float corrienteSuma = 0;
   for(int i = 0; i < numeroMuestras; i++) {
     leerAcs712 = analogRead(A0) * (5.00 / 1024.0);
     corrienteSuma += (leerAcs712 - 2.50) / sensibilidadcc;
   }
   float corriente = (corrienteSuma / numeroMuestras);
  //  if(corriente < 0.006 && corriente > -0.006 ) {
  //    corriente = 0;
  //  }
  //  if(corriente < 0.00) {
  //    corriente = corriente * -1;
  //  }
   return (corriente + 0.090);
}