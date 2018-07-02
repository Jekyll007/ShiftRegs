// Изменение значений разрядов семисегментного индикатора по нажатию на тактовые кнопки
// Подключение кнопок и индикатора осуществляется через сдвиговые регистры.
//----------------------------------------------------------------------------
// настройка линии подачи напряжения
int POWR = 2;     // питание +5В

// настройка пинов Arduino для выходного регистра
int LATCH_PIN_O = 8; // к 12 пину регистра - вход «защёлкивающий» данные
int DATA_PIN_O = 11;  // к 14 пину регистра - вход данных
int CLOCK_PIN_O = 13; // к 11 пину регистра - вход для тактовых импульсов

// настройка пинов Arduino для входного регистра
int LATCH_PIN_I = 9;  // к 1 пину регистра - вход «защёлкивающий» данные
int DATA_PIN_I = 12;  // к 9 пину регистра - вход данных
int CLOCK_PIN_I = 13;  // к 2 пину регистра - вход для тактовых импульсов

// описание кодов для сегментов индикатора
byte S_TOP = ~(1 << 1);  // A
byte S_RTOP = ~(1 << 2); // B
byte S_RBOT = ~(1 << 3); // C
byte S_BOT = ~(1 << 4);  // D
byte S_LBOT = ~(1 << 5); // E
byte S_LTOP = ~(1 << 6); // F
byte S_MID = ~(1 << 7);  // G
byte S_DP = ~(1 << 0);   // DP
byte S_EMPT = ~0;        // пустой

// массив значений кодов для отображения символов
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
  (byte) (S_TOP & S_MID & S_BOT & S_LTOP & S_LBOT),  // E
  (byte) (S_MID & S_LBOT),  // r
};

// массив значений разрядов индикатора (0-3)
byte ledDigs [4] = { 
  1 << 1, // 0
  1 << 2, // 1
  1 << 3, // 2
  1 << 4, // 3
};


void setup() {
  // настройка питания +5В на 6 пине Arduino
  pinMode(POWR, OUTPUT);
  digitalWrite(POWR, HIGH);
  // настройка портов для выходного регистра     
  pinMode(LATCH_PIN_O, OUTPUT);
  pinMode(DATA_PIN_O, OUTPUT);
  pinMode(CLOCK_PIN_O, OUTPUT);
  // настройка портов для входного регистра 
  pinMode(LATCH_PIN_I, OUTPUT);
  pinMode(CLOCK_PIN_I, OUTPUT);
  pinMode(DATA_PIN_I, INPUT);
  // включаем защёлку входного регистра
  digitalWrite(LATCH_PIN_O, HIGH);
}

void loop() { // бесконечный цикл
  static byte lastDataByte = 0;  // предыдущие состояния входов входного регистра
  static int dispNum = 0;
  int addNum = 0;  // слагаемое, добавляемое к отображаемому числу при каждом нажатии на кнопку
  byte dataByte = readByte();  // читаем состояния входного регистра
  if (dataByte != lastDataByte){  // если состояние любого входа изменилось (сравниваем с предыдущими) - обрабатываем байт
    if (dataByte > lastDataByte){  // если считанный текущее значение считанного байта больше предыдущего (реагируем только на нажатия)
      byte changedDataByte = dataByte ^ lastDataByte; // берём только изменившиеся биты
      // подбираем множитель, соответствующий изменившемуся биту
      int i = 0;  // счётчик разрядов в байте
      int m = 1;  // множитель разряда (1 10 100 1000 соотв.)
      while(!addNum){  // пока не подберём множитель
        if (changedDataByte & (1<<i)){  // если натыкаемся на ненулевой бит
          addNum = m;  // присваиваем добавляемому слагаемому множитель разряда, соответствующий найденному биту
        }
        i++;  // увеличивем счётчик разрядов
        m = m*10; // увеличиваем степень множителя 10^i
      }
      
    }
    lastDataByte = dataByte;  // запоминаем текущие состояния
  }
  dispNum = dispNum + addNum;  // добавляем соответствующее дополнительное слагаемое к отображаемому числу
  if (dispNum > 9999){  // если число превышает максимальное - сбрасываем его
    dispNum = 0;
  }
  ledDispNumSet(dispNum);  // отображаем число на индикаторе
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
  if (dig < 4 && dig >= 0) { // проверяем, существует ли разряд с номером dig 
    ledDispNumSet(num);  // отображаем число
    ledDispPoint(dig);     // отображаем точку
  } else {    // если номер разряда отрицательный или превышает их кол-во
    ledDispNumSet(num);  // отображаем только число
  }
}

//----------------------------------------------------------------------------
// ВХОДНОЙ РЕГИСТР
// чтение байта с входного регистра
byte readByte(){
  byte retByte = 0;  // возвращаемый байт
  digitalWrite(CLOCK_PIN_I,HIGH);  // необходимо для корректной работы shiftIn()
  digitalWrite(LATCH_PIN_I, LOW);  // "дёргаем" защёлку для запоминания регистром
  digitalWrite(LATCH_PIN_I, HIGH);  // уровней сигналов на входах
  retByte = shiftIn(DATA_PIN_I,CLOCK_PIN_I,MSBFIRST); // читаем байт с регистра
  return retByte;
}

