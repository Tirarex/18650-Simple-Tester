

float Resistor = 5.6; //Сопротивление основного резистора
float R1 = 67.6; //Сопротивление нижнего плеча делителя напряжение
float R2 = 670.0; //Сопротивление верхнего плеча делителя напряжение
float LowBat = 3.0; //Нижний порог рассчета напряжения
int DataPin = 0; //Аналоговый порт с которого ардуина будет читать напряжение

//Служебные переменные, не трогать
int Time = 0;
bool TestEnd = false;
bool LightState = 0;
float Volt = 0;
float Current = 0;
float Ah = 0;
float Vbat = 0;


void setup()
{
  //Настраиваем индикатор
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  //Включаем внутренний источник опорного напряжения
  analogReference(INTERNAL);


  // Печатаем тексты
  Serial.begin(9600);
  Serial.println("18650 EasyTest");
  Serial.println("Bat ?");
}

// Собственно расчёты и вывод на дисплей
void loop()
{
  Volt = getOversampled(1); // Читаем напряжение

  if ( Volt <= LowBat )
  {
    LightState = 1 - LightState;
    digitalWrite(13, LightState);
    TestEnd = true;
    Serial.println("Otkluchi Battareu");



    delay (1000);
  }

  if (TestEnd == false)
  {
    if ( Volt >= LowBat )
    {
      digitalWrite(13, HIGH);
      Volt = getOversampled(90); // Читаем усредненное напряжение (90 выборок)
      Current = Volt / Resistor; // Рассчитываем ток по формуле I=u/r
      Ah = Ah + Current / 3600; //Добавляем к емкости сожранные мА

      Time++; //Добавляем секунду к таймеру

      // Выводим напряжение на батарее
      Serial.println("V:");
      Serial.print(Volt);

      // Выводим время теста
      Serial.print("  Time:");
      Serial.print(Time);
      //Serial.print(Time/60); //Раскоментировать для вывода времени в минутах

      // Выводим емкость
      Serial.print("  mAh:");
      Serial.print(Ah * 1000);

      //Выводим текущий ток цепи
      Serial.print("  A:");
      Serial.print(Current);
    }
  }
}

//Функция получения напряжения батареи
float getOversampled(int count) {
  float arefv = 0;
  float vout = 0;
  for (int i = 0; i < count; i++) {
    arefv = (analogRead(A0) * 1.1) / 1023.0; //Читаем сырое значение и умножаем на ширину ацп
    vout = vout + (arefv / (R1 / (R2 + R1))); //Делим сырое значение с учетом номинала резисторного делителя и добавляем к результату
    delay (10); //ждем 10 милисекунд
  }
  return vout / count ; //Делим результат на кол-во измерений
}

