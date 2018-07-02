// Отображение информации на 4-разрядном, 7-сегментном LED-индикаторе, общий анод,
// подключение через 2 сдвиговых регистра 74HC595N.
//----------------------------------------------------------------------------
// настройка линии подачи напряжения
int POWR = 2;     // питание +5В

// настройка пинов Arduino для выходного регистра
int LATCH_PIN_O = 8; // к 12 пину регистра - вход «защёлкивающий» данные
int DATA_PIN_O = 11;  // к 14 пину регистра - вход данных
int CLOCK_PIN_O = 13; // к 11 пину регистра - вход для тактовых импульсов

// значения байта сигналов для соотв. сегментов индикатора
byte S_TOP = ~(1 << 1);  // A сегменты индикатора
byte S_RTOP = ~(1 << 2); // B
byte S_RBOT = ~(1 << 3); // C
byte S_BOT = ~(1 << 4);  // D
byte S_LBOT = ~(1 << 5); // E
byte S_LTOP = ~(1 << 6); // F
byte S_MID = ~(1 << 7);  // G
byte S_DP = ~(1 << 0);   // DP
byte S_EMPT = ~0;        // пустой

// массив значений байта сигналов для отображения символа
// массив получается путём объединения значений для отдельных сегментов
byte ledChars[14] = { 
  (byte) (S_TOP & S_BOT & S_LTOP & S_LBOT & S_RTOP & S_RBOT),  // 0
  (byte) (S_RTOP & S_RBOT),  // 1
  (byte) (S_TOP & S_MID & S_BOT & S_LBOT & S_RTOP),  // 2
  (byte) (S_TOP & S_MID & S_BOT & S_RTOP & S_RBOT),  // 3
  (byte) (S_MID & S_LTOP & S_RTOP & S_RBOT),  // 4
  (byte) (S_TOP & S_MID & S_BOT & S_LTOP & S_RBOT),  // 5
  (byte) (S_TOP & S_MID & S_BOT & S_LTOP & S_LBOT & S_RBOT),  // 6
  (byte) (S_TOP & S_RTOP & S_RBOT),  // 7
  (byte) (S_TOP & S_MID & S_BOT & S_LTOP & S_LBOT & S_RTOP & S_RBOT),  // 8
  (byte) (S_TOP & S_MID & S_BOT & S_LTOP & S_RTOP & S_RBOT),  // 9
  (byte) (S_DP),  // .
  (byte) (S_EMPT),  // пусто
  (byte) (S_TOP & S_MID & S_BOT & S_LTOP & S_LBOT & S_DP),  // E
  (byte) (S_MID & S_LBOT),  // r
};

// массив значений для байта управления выбором разряда
byte ledDigs [4] = { // разряды индикатора
  1 << 1, // 0
  1 << 2, // 1
  1 << 3, // 2
  1 << 4, // 3
};


void setup() {
// настройка портов на выход      
  pinMode(LATCH_PIN_O, OUTPUT);
  pinMode(DATA_PIN_O, OUTPUT);
  pinMode(CLOCK_PIN_O, OUTPUT);
// настройка питания +5В на 6 пине Arduino
  pinMode(POWR, OUTPUT);
  digitalWrite(POWR, HIGH);
}


void loop() { // бесконечный цикл
  dispInc(200);
}

//----------------------------------------------------------------------------
// ПОЛЬЗОВАТЕЛЬСКИЕ ФУНКЦИИ
// Отображение инкрементирования
void dispInc(int t) {  // период инкрементирования
  for (int i = 0; i < 10000; i++) {
    for (int j = 0; j < t; j++) {
      ledDispNumSet(i);
    }
  } 
}

//----------------------------------------------------------------------------
//  ИНДИКАТОР
// отображение произвольного символа
void ledDisp(byte symbCode, byte posCode) {  // катоды, аноды
  digitalWrite(LATCH_PIN_O, LOW);  
  shiftOut(DATA_PIN_O, CLOCK_PIN_O, MSBFIRST, symbCode);  // отправляем байт для катодов
  shiftOut(DATA_PIN_O, CLOCK_PIN_O, MSBFIRST, posCode);   // отправляем байт для анодов
  digitalWrite(LATCH_PIN_O, HIGH); // "защёлкиваем" данные
}

// отображение символа в соотв. с массивом 
void ledDispChar(int num, int dig) {  // индекс символа в массиве, разряд на котором отображать
  ledDisp(ledChars[num], ledDigs[dig]);
}

// отображение точки в указанном разряде
void ledDispPoint(int dig) {  // разряд для отображения точки
  ledDisp(ledChars[10], ledDigs[dig]);
}

// отображение ошибки
void ledDispErr() {  
  ledDispChar(13, 1); // символ E
  ledDispChar(13, 2); // символ r
  ledDispChar(12, 3); // символ r
}

// отображение 4-х значного числа (динамическая индикация)
void ledDispNumSet(int x) {  // 4-х значное число
  if (x == 0) {  // если число 0 - сразу выводим 0
    ledDispChar(0, 0);
    return;
  }
  if (x > 9999) {  // если число > 4 знаков - выводим ошибку
    ledDispErr();
    return;
  }
  int nums[4] = {11, 11, 11, 11}; // массив пустых символов
  int i = 0;
  while (x > 0) {   // разбиение по разрядам в массив nums
    nums[i] = x % 10;
    x = x / 10;
    i++;
  }
  for (int i = 0; i < 4; i++) {   // вывод получившегося массива nums на индикатор
    ledDispChar(nums[i], i);
  }
}

// отображение числа с точкой в указанном разряде
void ledDispNumSetWithPoint(int num, int dig) {  // число для отображения, разряд для точки  
  if (dig < 4 && dig >= 0) { // проверяем, соотв. ли номер существующему разряду 
    ledDispNumSet(num);  // отображаем число
    ledDispPoint(dig);     // отображаем точку
  } else {    // если номер разряда отрицательный или превышает их кол-во
    ledDispNumSet(num);  // отображаем только число
  }
}

