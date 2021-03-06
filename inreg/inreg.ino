// Отправка в терминал состояний 4 тактовых кнопок, подключенных к
// входному сдвиговому регистру 74HC165.
//----------------------------------------------------------------------------
// настройка линии подачи напряжения
int POWR = 2;     // питание +5В

// настройка пинов Arduino для входного регистра
int LATCH_PIN_I = 9;  // к 1 пину регистра - вход «защёлкивающий» данные
int DATA_PIN_I = 12;  // к 9 пину регистра - вход данных
int CLOCK_PIN_I = 13;  // к 2 пину регистра - вход для тактовых импульсов

void setup() {  
  // настройка портов на выход
  pinMode(LATCH_PIN_I, OUTPUT);
  pinMode(CLOCK_PIN_I, OUTPUT);
  // настройка портов на вход
  pinMode(DATA_PIN_I, INPUT);
  // включаем защёлку
  digitalWrite(LATCH_PIN_I, HIGH);
  // открываем последовательный порт, устанавливаем скорость 9600 бод
  Serial.begin(9600); 
}

void loop() {  // бесконечный цикл
  static byte lastDataByte = 0;  // предыдущие состояния входов регистра
  byte dataByte = readByte();  // читаем  текущие состояние входов регистра
  if (dataByte != lastDataByte) {  // если состояние любого входа изменилось (сравниваем с предыдущими)
    Serial.println(dataByte,BIN);  // отправляем состояния входов в терминал
    lastDataByte = dataByte;  // запоминаем текущие состояния
  }
}

//----------------------------------------------------------------------------
// СДВИГОВЫЙ РЕГИСТР
// чтение байта с входного регистра
byte readByte() {
  digitalWrite(CLOCK_PIN_I,HIGH);  // необходимо для корректной работы shiftIn()
  digitalWrite(LATCH_PIN_I, LOW);  // "дёргаем" защёлку для запоминания регистом
  digitalWrite(LATCH_PIN_I, HIGH);  // уровней сигналов на входах
  byte retByte = shiftIn(DATA_PIN_I,CLOCK_PIN_I,MSBFIRST); // читаем байт с регистра
  return retByte;
}

