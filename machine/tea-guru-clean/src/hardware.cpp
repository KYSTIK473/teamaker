#include "hardware.h"


// Используемое железо
OneWire ds(A0);
PCF8574 pcf(0x20);
CustomStepper stepper_tea(pcf, 0, 1, 2, 3);
CustomStepper stepper_sugar(pcf, 4, 5, 6, 7);
GButton butt1(A3);
SerialTea serialTea(Serial);


// Настойка железа пинов и прочего
void setupHardware()
{
  Serial.begin(115200);
  pinMode(PIN_HOLOD, OUTPUT); // Помпа под холодную воду
  digitalWrite(PIN_HOLOD, LOW);
  
  pinMode(PIN_KIP, OUTPUT); // Помпа под горячую воду
  digitalWrite(PIN_KIP, LOW);

  pinMode(PIN_TEN, OUTPUT); // Реле включения тена
  digitalWrite(PIN_TEN, LOW);

  pinMode(PIN_KRAN, OUTPUT);
  digitalWrite(PIN_KRAN, LOW);

  pinMode(PIN_TRIG, OUTPUT);
  digitalWrite(PIN_TRIG, LOW);

  pinMode(PIN_ECHO, INPUT);// Реле открытия крана
  digitalWrite(PIN_ECHO, LOW);
  
  // Инициализация моторчиков
  stepper_tea.setRPM(10);//12);
  stepper_tea.setSPR(4075.7728395);
  stepper_tea.setDirection(CCW);
  stepper_sugar.setRPM(10);//12);
  stepper_sugar.setSPR(4075.7728395);
  stepper_sugar.setDirection(CCW);

  stepper_tea.disable();
  stepper_sugar.disable();

  // Короткий таймаут для входящих команд
  Serial.setTimeout(30);
}


float getBoilerTemp()
{
  byte data[2]; // Место для значения температуры
  ds.reset(); // Начинаем взаимодействие со сброса всех предыдущих команд и параметров
  ds.write(0xCC); // Даем датчику DS18b20 команду пропустить поиск по адресу. В нашем случае только одно устрйоство 
  ds.write(0x44); // Даем датчику DS18b20 команду измерить температуру. Само значение температуры мы еще не получаем - датчик его положит во внутреннюю память
  
  delay(1000); // Микросхема измеряет температуру, а мы ждем
  
  ds.reset(); // Теперь готовимся получить значение измеренной температуры
  ds.write(0xCC); 
  ds.write(0xBE); // Просим передать нам значение регистров со значением температуры

  // Получаем и считываем ответ
  data[0] = ds.read(); // Читаем младший байт значения температуры
  data[1] = ds.read(); // А теперь старший

  // Формируем итоговое значение: 
  //    - сперва "склеиваем" значение,  
  //    - затем умножаем его на коэффициент, соответсвующий разрешающей способности (для 12 бит по умолчанию - это 0,0625)
  float getTemp =  ((data[1] << 8) | data[0]) * 0.0625;
  //Serial.println(getTemp);
  return getTemp;
}


int cupDistance()
{
  long cm = 200;
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_TRIG, HIGH);
  
  // Выставив высокий уровень сигнала, ждем около 10 микросекунд. В этот момент датчик будет посылать сигналы с частотой 40 КГц.
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  
  //  Время задержки акустического сигнала на эхолокаторе.
  auto duration = pulseIn(PIN_ECHO, HIGH);
  
  // Теперь осталось преобразовать время в расстояние
  cm = (duration / 2) / 29.1;
  
  //Serial.print("Расстояние до объекта: ");
  //Serial.print(cm);
  //Serial.println(" см.");
  
  return cm;
}


void setBoilder(bool enabled)
{
  //digitalWrite(PIN_TEN, LOW); // Debug
  digitalWrite(PIN_TEN, enabled ? HIGH : LOW);
}


void setColdPump(bool enabled)
{
  digitalWrite(PIN_HOLOD, enabled ? HIGH : LOW);
}


void setHotPump(bool enabled)
{
  digitalWrite(PIN_KIP, enabled ? HIGH : LOW);
}


void startTeaStepper(int portions)
{
  stepper_tea.rotateDegrees(90 * portions);
}


void startSugarStepper(int portions)
{
  stepper_sugar.rotateDegrees(90 * portions);
}


void setDispenser(bool opened)
{
  digitalWrite(PIN_KRAN, opened ? HIGH : LOW);
}
