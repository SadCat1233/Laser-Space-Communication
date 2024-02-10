#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9, 10);  // "создать" модуль на пинах 9 и 10 Для Уно

bool flag = true;

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

void setup() {
  Serial.begin(9600);         // открываем порт для связи с ПК
  radio.begin();              // активировать модуль
  radio.setPayloadSize(32);   // размер пакета, в байтах

  radio.openReadingPipe(1, address[0]);   // хотим слушать трубу 0
  radio.setChannel(0x60);     // выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX);   // уровень мощности передатчика
  radio.setDataRate (RF24_250KBPS); // скорость обмена

  radio.powerUp();        // начать работу
  radio.startListening(); // начинаем слушать эфир, мы приёмный модуль
}

void loop() {
  if (Serial.available() and flag) {     //отправка команда для перзваписи среднего значени освещенности
    flag = !flag;
    radio.closeReadingPipe(address[0]);
    radio.stopListening();
    radio.openWritingPipe(address[1]);
    byte output_ = 1;
    radio.write(&output_, sizeof(output_));
    Serial.println("Sent");
    radio.startListening();
    radio.openReadingPipe(1, address[0]);
  }
  byte pipeNo;
  char gotByte;
  while (radio.available(&pipeNo)) {        // слушаем эфир со всех труб
    radio.read(&gotByte, sizeof(gotByte));  // чиатем входящий сигнал
    Serial.print(gotByte);
  }
}