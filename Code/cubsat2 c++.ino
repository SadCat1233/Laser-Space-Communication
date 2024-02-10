#define BIT_DELAY 40 // Задержка между битами
#define DELAY_BETWEEN_MESSAGES 500 // Задержка между отправкой двух сообщений
#define NUM_READ 500
#include <SPI.h>          // библиотека для работы с шиной SPI
#include "nRF24L01.h"     // библиотека радиомодуля
#include "RF24.h"         // ещё библиотека радиомодуля

RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

String input_message = "";
char output_char;
int clear_signal = 0;

static int buffer[NUM_READ];

const int numReadings = 100;

static int buf[3];

int readings[numReadings];
int readIndex = 0;
long int total = 0;
int average = 0;

int inputPin = A0;


void setup() {
  Serial.begin(9600);
  pinMode(inputPin, INPUT);

  radio.begin();              // активировать модуль
  radio.setPayloadSize(32);   // размер пакета, в байтах

  radio.openReadingPipe(1, address[0]);  // мы - труба 0, открываем канал для передачи данных
  radio.setChannel(0x60);             // выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX);   // уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); // скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS

  radio.powerUp();        // начать работу
  radio.startListening();  // не слушаем радиоэфир, мы передатчик
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  funct();
  Serial.println("-----");
  Serial.println(average);
  int a = 1024 - analogRead(inputPin);
  byte pipeNo;
  while (a - average < 50) {
    if (radio.available(&pipeNo)) {
      total = 0;
      readIndex = 0;
      funct();
    }
    a = 1024 - analogRead(inputPin);
  }
  radio.closeReadingPipe(address[0]);
  radio.openWritingPipe(address[0]);
  radio.stopListening();
}


void loop() {
  int lastReceiveTime = 0;
  if (1024 - analogRead(inputPin) - average < 25) {
    input_message += "0";
    for (int i = 0; i < 10; i++) {
      delay(BIT_DELAY);
      if (1024 - analogRead(inputPin) - average < 25) {
        input_message += "0";
      }
      else {
        input_message += "1";
      }
    }
    String sss = input_message.substring(1, 9);
    output_char = char(strtol(sss.c_str(), NULL, 2));
    radio.write(&output_char, sizeof(output_char));
    int lastReceiveTime = millis();
    input_message = "";
  }
}


int median(int newVal) {
  buf[2] = newVal;
  int middle = (max(buf[0], buf[1]) == max(buf[1], buf[2])) ? max(buf[0], buf[2]) : max(buf[1], min(buf[0], buf[2]));
  Serial.println(middle);
  buf[0] = buf[1];
  buf[1] = buf[2];
  return middle;
}


int funct() {
  buf[0] = 1024 - analogRead(inputPin);
  buf[1] = 1024 - analogRead(inputPin);
  while (readIndex < numReadings) {
    readings[readIndex] = median(1024 - analogRead(inputPin));
    total += readings[readIndex];
    readIndex = readIndex + 1;
    delay(20);
  }
  average = total / numReadings;
}