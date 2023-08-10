#define PIN_TRIG 8
#define PIN_ECHO 9

long duration, cm;

int zn, c;
float mid;

void setup() {

  // Инициализируем взаимодействие по последовательному порту

  Serial.begin (9600);
  //Определяем вводы и выводы
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
}


void loop() {
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
   delay(10000);
}
