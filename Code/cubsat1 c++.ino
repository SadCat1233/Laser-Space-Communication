#define BIT_DELAY 40 // Задержка между битами
#define DELAY_BETWEEN_MESSAGES 500 // Задержка между отправкой двух сообщений


void send_bit(bool b) {
  digitalWrite(11, b); // Стартовый бит
  Serial.print(b ? "1" : "0");
  delay(BIT_DELAY);
}


void set_idle() {
  digitalWrite(11, 1); // IDLE
  delay(DELAY_BETWEEN_MESSAGES);
}


void setup() {
  Serial.begin(9600);
  pinMode(11, OUTPUT);
  set_idle();\
}


void transmit(String message) {
  for (int i = 0; i < message.length(); i++) { // Проходимся по символам
    send_bit(0); // Стартовый бит
    int parity_count = 0; // Счетчик для вычисления бита четности
    for (int j = 0; j < 8; j++) { // Проходимся по битам символа
      bool _bit = message[i] & 0b10000000 ? 1 : 0; // Получаем старший бит символа
      if (_bit) parity_count++; // Если он 1, то добавляем к счетчику бита четности
      send_bit(_bit); // Передаем бит
      message[i] = message[i] << 1; // Сдвигаем биты символа на один влево, чтобы в следующий раз получить следующий бит
    }
    Serial.println("");
    send_bit(parity_count % 2); // Вычисляем бит четности и отправляем
    send_bit(1); // Стоповый бит
  }
  delay(DELAY_BETWEEN_MESSAGES);
}


void loop() {
  transmit("X:0528 Y:0723 Z:0129 \n");
  transmit("Vx:+02 Vy:+04 Vz:-07 \n"); //1340 +- 100
  transmit("Battery: 95% \n"); //1020 +- 100
}