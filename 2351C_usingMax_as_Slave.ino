/*
  ModbusRTU ESP8266/ESP32
  Simple slave example

  (c)2019 Alexander Emelianov (a.m.emelianov@gmail.com)
  https://github.com/emelianov/modbus-esp8266

  modified 13 May 2020
  by Niladri Dey

  .
*/
#include "DHT20.h"


#include <ModbusRTU.h>

#define REGN 1
#define SLAVE_ID 1
#define RXTX_PIN 14
#define LED_SUCCESS    2
#define LED_SENSOR_READ    15
#define LED_MODBUS_CONNECTED    0

ModbusRTU mb;
DHT20 DHT(&Wire);

void setup() {
  Serial.begin(9600, SERIAL_8N1);
#if defined(ESP32) || defined(ESP8266)
  mb.begin(&Serial,RXTX_PIN);
  mb.setBaudrate(9600);
#else
  mb.begin(&Serial,RXTX_PIN);
  //mb.begin(&Serial, RXTX_PIN);  //or use RX/TX direction control pin (if required)
  mb.setBaudrate(9600);
#endif
  delay(2000);
  mb.onRequestSuccess(cbPostRequest);
  mb.slave(SLAVE_ID);
  mb.addHreg(1);
  //mb.Hreg(0, 1);
  mb.addHreg(2);
  //mb.addHreg(2);
  //mb.Hreg(1, 1);
  delay(1000);

  // for sensor 
  #if defined(ESP8266) || defined(ESP32)
  DHT.begin(4, 5);  //  select your pin numbers here
#else
  DHT.begin();
#endif
  pinMode(LED_SUCCESS, OUTPUT);
  pinMode(LED_SENSOR_READ, OUTPUT);
  pinMode(LED_MODBUS_CONNECTED, OUTPUT);
  Serial.print("All Set"); 
  delay(2000);
}
Modbus::ResultCode cbPostRequest(Modbus::FunctionCode fc, const Modbus::RequestData data) {
  //Serial.printf("POST Function: %02X\n", fc);
  led_blink(2,LED_MODBUS_CONNECTED);
  return Modbus::EX_SUCCESS;
}
void led_blink(int blk, int location){
   for (int j=0; j < blk ;j++){
          digitalWrite(location, HIGH);delay(30);digitalWrite(location,LOW);delay(30);
        }
}

void loop() {
  if (mb.slave()) {
    //mb.Hreg(0, 2);
    //mb.Hreg(1, 2);
    int status = DHT.read();
    if(status == DHT20_OK) {
      //Serial.print("OK,\t");
      float temperature = DHT.getTemperature()*100;
      float humidity = DHT.getHumidity()*100;
      mb.Hreg(1, temperature);
      mb.Hreg(2, humidity);
      //Serial.println(temperature);
      //Serial.println(humidity);
      // Blynk LED to know sensor working fine 
      led_blink(1,LED_SENSOR_READ);
    }
  }
  mb.task();
  yield();
}
