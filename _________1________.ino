  
#include "GyverButton.h"
#include <OneWire.h>
#include <CustomStepper.h>
/*
* Описание взаимодействия с цифровым датчиком ds18b20 
* Подключение ds18b20 к ардуино через пин 8  
*/
OneWire ds(A4);
GButton butt1(A3);
CustomStepper stepper(2, 3, 4, 5, (byte[]){8, B1000, B1100, B0100, B0110, B0010, B0011, B0001, B1001}, 4075.7728395, 12, CW);

float temperature = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(2, OUTPUT); // pomp holod
  pinMode(3, OUTPUT); // pomp gor
  pinMode(12, OUTPUT); // rele gor
  pinMode(A5, OUTPUT); // rele kran
  stepper.setRPM(12);
  //sets the Steps Per Rotation, in this case it is 64 * the 283712/4455 annoying ger ratio
  //for my motor (it works with float to be able to deal with these non-integer gear ratios)
  stepper.setSPR(4075.7728395);
  digitalWrite(12, HIGH);
  digitalWrite(2, LOW);
  
}

void loop() {
  butt1.tick();
  
  if (butt1.isSingle()){ // кружка чая
    digitalWrite(2, HIGH); // holod
    delay(14000); // zhdem
    digitalWrite(2, LOW);
    digitalWrite(12, LOW); // nach kipachenie
    while (temperature < 60){
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
      temperature =  ((data[1] << 8) | data[0]) * 0.0625;
      Serial.println(temperature);
    }
    digitalWrite(12, HIGH); // zak kipch
    stepper.setDirection(CW); // dobavin chayu
    //this method makes the motor rotate a given number of degrees, it works with float
    //you can give angles like 90.5, but you can't give negative values, it rotates to the direction currently set
    stepper.rotateDegrees(90);
    stepper.run();
    delay(1000);
    digitalWrite(3, HIGH); // perelion kipatok
    delay(15000);
    digitalWrite(3, LOW);
    delay(50000); // zdem zavarky
    digitalWrite(A5, LOW); // vilivaem
    delay(20000);
    digitalWrite(A5, HIGH);
  }


  
  if (butt1.isDouble()){  // chainek
    Serial.println("Double");
    digitalWrite(2, HIGH); // holod
    delay(30000); // zhdem
    digitalWrite(2, LOW);
    digitalWrite(12, HIGH); // nach kipachenie
    while (temperature < 60){
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
    }
    digitalWrite(12, HIGH); // zak kipch
    stepper.setDirection(CW); // dobavin chayu
    //this method makes the motor rotate a given number of degrees, it works with float
    //you can give angles like 90.5, but you can't give negative values, it rotates to the direction currently set
    stepper.rotateDegrees(90);
    stepper.run();
    delay(1000);
    digitalWrite(3, HIGH); // perelion kipatok
    delay(31000);
    digitalWrite(3, LOW);
    delay(50000); // zdem zavarky
    digitalWrite(A5, LOW); // vilivaem
    delay(30000);
    digitalWrite(A5, HIGH);
  }
  if (butt1.isTriple()) { // promyvka
    Serial.println("Triple");
    digitalWrite(2, HIGH); // holod
    delay(14000); // zhdem
    digitalWrite(2, LOW);
    digitalWrite(3, HIGH); // perelion kipatok
    delay(15000);
    digitalWrite(3, LOW);
    digitalWrite(A5, LOW); // vilivaem
    delay(20000);
    digitalWrite(A5, HIGH);
  }
}
