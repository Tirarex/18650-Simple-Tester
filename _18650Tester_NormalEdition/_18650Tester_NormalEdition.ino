
#include <Wire.h> ]
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x38, 16, 2);


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

  //Инициализируем дисплей
  lcd.begin();
  lcd.backlight();

  // Печатаем тексты
  lcd.print("18650 EasyTest");
  delay (100);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bat ?");
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
    delay (100);
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
      lcd.setCursor(0, 0);
      lcd.print("V:");
      lcd.setCursor(2, 0);
      lcd.print(Volt);

      // Выводим время теста
      lcd.setCursor(7, 0);
      lcd.print("Time:");
      lcd.setCursor(11, 0);
      lcd.print(Time);
      //lcd.print(Time/60); //Раскоментировать для вывода времени в минутах

      // Выводим емкость
      lcd.setCursor(0, 1);
      lcd.print("mAh:");
      lcd.setCursor(3, 1);
      lcd.print(Ah * 1000);

      //Выводим текущий ток цепи
      lcd.setCursor(10, 1);
      lcd.print("A:");
      lcd.setCursor(12, 1);
      lcd.print(Current);
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

