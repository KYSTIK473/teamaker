#include "pcf8574.h"
#include "CustomStepper.h"
#include <OneWire.h>
/*
* Описание взаимодействия с цифровым датчиком ds18b20 
* Подключение ds18b20 к ардуино через пин 8  
*/
OneWire ds(A0);
#define PIN_TRIG 8
#define PIN_ECHO 9

long duration, cm;

int zn, c;
float mid;
int st = 1;
PCF8574 pcf(0x20);
CustomStepper stepper(pcf, 0, 1, 2, 3);
CustomStepper stepper2(pcf, 4, 5, 6, 7);
void setup() {
  Serial.begin (9600);
  stepper.setRPM(6);
  stepper.setSPR(4075.7728395);
  stepper2.setRPM(6);
  stepper2.setSPR(4075.7728395);
  //stepper.disable();
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  digitalWrite(A4, LOW);  
 digitalWrite(A5, LOW);
}

void loop() {
  if (st == 1){
    digitalWrite(A4, LOW);  
    digitalWrite(A5, LOW);
    // rast 
    mid = 0;
  zn = 0;
  c = 0;
  // Сначала генерируем короткий импульс длительностью 2-5 микросекунд.
  for (int i=0; i <= 10; i++){
      digitalWrite(PIN_TRIG, LOW);
      delayMicroseconds(5);
      digitalWrite(PIN_TRIG, HIGH);
    
      // Выставив высокий уровень сигнала, ждем около 10 микросекунд. В этот момент датчик будет посылать сигналы с частотой 40 КГц.
      delayMicroseconds(10);
      digitalWrite(PIN_TRIG, LOW);
    
      //  Время задержки акустического сигнала на эхолокаторе.
      duration = pulseIn(PIN_ECHO, HIGH);
    
      // Теперь осталось преобразовать время в расстояние
      cm = (duration / 2) / 29.1;
    
      Serial.print("Расстояние до объекта: ");
      Serial.print(cm);
      Serial.println(" см.");
      if (cm <= 60) {
        zn = zn + cm; 
        c++;
      }
    
      // Задержка между измерениями для корректной работы скеча
      delay(250);
   }
   mid = zn / c;
   Serial.print(mid);
   delay(3000);

   // temp
  for (int i=0; i <= 10; i++){
    byte data[2]; // Место для значения температуры
  
    ds.reset(); // Начинаем взаимодействие со сброса всех предыдущих команд и параметров
    ds.write(0xCC); // Даем датчику DS18b20 команду пропустить поиск по адресу. В нашем случае только одно устрйоство 
    ds.write(0x44); // Даем датчику DS18b20 команду измерить температуру. Само значение температуры мы еще не получаем - датчик его положит во внутреннюю память
    
    delay(1000); // Микросхема измеряет температуру, а мы ждем.  
    
    ds.reset(); // Теперь готовимся получить значение измеренной температуры
    ds.write(0xCC); 
    ds.write(0xBE); // Просим передать нам значение регистров со значением температуры
  
    // Получаем и считываем ответ
    data[0] = ds.read(); // Читаем младший байт значения температуры
    data[1] = ds.read(); // А теперь старший
  
    // Формируем итоговое значение: 
    //    - сперва "склеиваем" значение, 
    //    - затем умножаем его на коэффициент, соответсвующий разрешающей способности (для 12 бит по умолчанию - это 0,0625)
    float temperature =  ((data[1] << 8) | data[0]) * 0.0625;
    
    // Выводим полученное значение температуры в монитор порта
    Serial.println(temperature);
    }
    delay(100);
    Serial.print("Проверка помп");
    for (int i=0; i <= 2; i++){
    digitalWrite(A4, HIGH);  
    digitalWrite(A5, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(3000);                       // wait for a second
    digitalWrite(A4, LOW);  
    digitalWrite(A5, LOW);    // turn the LED off by making the voltage LOW
    delay(3000);  
    }// wait for a second
    st = 2;
    Serial.print("этап завершён");
  }
  digitalWrite(A4, LOW);  
  digitalWrite(A5, LOW); 
  // put your main code here, to run repeatedly:
  stepper.setDirection(CW);
  //this method sets the motor to rotate a given number of times, if you don't specify it, 
  //the motor will rotate untilyou send another command or set the direction to STOP
  stepper.rotateDegrees(180);
  stepper2.setDirection(CCW);
  //this method sets the motor to rotate a given number of times, if you don't specify it, 
  //the motor will rotate untilyou send another command or set the direction to STOP
  stepper2.rotateDegrees(180);
  stepper2.run();
  stepper.run();
}
