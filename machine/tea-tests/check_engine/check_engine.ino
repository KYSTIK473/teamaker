


// Используемое железо
CustomStepper stepper_tea(pcf, 0, 1, 2, 3);
CustomStepper stepper_sugar(pcf, 4, 5, 6, 7);


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


void startTeaStepper(int portions)
{
  stepper_tea.rotateDegrees(90 * portions);
}


void startSugarStepper(int portions)
{
  stepper_sugar.rotateDegrees(90 * portions);
}

startTeaStepper(4);
startSugarStepper(4);
