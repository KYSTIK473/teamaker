#include <Arduino.h>


#include "SerialTea.h"
#include "hardware.h"


// Этапе приготовления чая
enum class TeaState
{
  idle,             // Мы не готовим чай
  starting,         // Мы начинаем готовить чай
  preparing_water,  // Налив воды в кипятильник
  boiling_water,    // Кипячение
  start_steppers,   // Начинаем насыпать всякое
  adding_tea_and_sugar, // Насыпаем чай и сахар в заварник 
  adding_water,     // Заливаем воду в заварник
  brewing,          // Завариваем
  cleanning_up,     // Финализация
};


static TeaParams params;
static TeaState tState = TeaState::idle;
static unsigned long deadline;


void teaLoop();


void setup()
{
  setupHardware();

  // Ручной переход на нужный этап
  tState = TeaState::idle;

  Serial.println("Booting!");
}


void loop()
{
  // Запуск программы
  butt1.tick();
  serialTea.poll();
  stepper_tea.run();
  stepper_sugar.run();

  if (tState == TeaState::idle)
  {
    if (serialTea.teaRequested)
    {
      serialTea.teaRequested = false;
      params = serialTea.params;
      tState = TeaState::starting;
    }

    if (butt1.isSingle()){
      tState = TeaState::starting; // начинаем работу
    }
  }

  teaLoop(); // отрабатываем алгоритм в зависимости от nowStep

}


void teaLoop()
{
  switch (tState)
  {
    case TeaState::idle:
      break;

    case TeaState::starting: {
      // Начинаем! Льем воду
      const auto msToWait = params.waterAmount * COLD_PUMP_MS_PER_ML;
      Serial.print("starting! poring water for ");
      Serial.print(params.waterAmount);
      Serial.print("ml/");
      Serial.print(msToWait);
      Serial.println("ms");
      deadline = millis() + msToWait;
      setColdPump(true);
      tState = TeaState::preparing_water;
    } break;

    case TeaState::preparing_water: {
      // Ждем пока вода нальется
      if (millis() >= deadline)
      {
        // Перестаем лить
        setColdPump(false);
        // Начинаем греть воду
        Serial.println("boiling!");
        setBoilder(true);
        deadline = millis() + HEATER_MAX_TIME;
        tState = TeaState::boiling_water;
      }
    } break;

    case TeaState::boiling_water: {
      const auto tillDeadline = deadline - millis();
      const auto temp = getBoilerTemp();
      Serial.print("Temp is: ");
      Serial.print(temp);
      Serial.print("/");
      Serial.print(params.heatTemperature);
      Serial.print("C, time untill deadline ");
      Serial.print(tillDeadline);
      Serial.println("ms");

      if (millis() >= deadline)
      {
        // Мы слишком долго греем, ГАЛЯ ОТМЕНА
        Serial.println("ABORT ABORT!");
        tState = TeaState::start_steppers; //TeaState::idle;
      }
      else if (temp >= params.heatTemperature)
      {
        Serial.println("temperature reached, adding tea and sugar");
        tState = TeaState::start_steppers;
      }

      if (tState != TeaState::boiling_water)
      {
        // Если выходим из этого состояния - глушим кипятильник
        setBoilder(false);
      }
    } break;

    case TeaState::start_steppers:
      // Запускаем шаговики
      Serial.print("adding tea ");
      Serial.print(params.teaPortions);
      Serial.print(" and sugar ");
      Serial.println(params.sugarPortions);
      // Запускаем шаговики для чая и сахара
      startTeaStepper(params.teaPortions);
      startSugarStepper(params.sugarPortions);
      Serial.println("steppers started");
      tState = TeaState::adding_tea_and_sugar;
      break;

    case TeaState::adding_tea_and_sugar: {
      // Ждем пока докрутятся шаговики
      if (stepper_tea.isDone() && stepper_sugar.isDone())
      {
        // Запускаем насос с кипятком
        deadline = millis() + params.waterAmount * HOT_PUMP_MS_PER_ML;
        setHotPump(true);
        tState = TeaState::adding_water;
      }
    } break;

    case TeaState::adding_water: {
      // Ждем пока вода нальется
      if (millis() >= deadline)
      {
        // Глушим насос
        setHotPump(false);
        Serial.println("Brewing...");
        deadline = millis() + BREW_TIME;
        tState = TeaState::brewing;
      }
    } break;

    case TeaState::brewing: {
      // Ждем пока чай заварится
      if (millis() >= deadline)
      {
        // Заварилось // Работаем только если чашка стоит
        const auto distance = cupDistance();
        if (distance <= 8)
        {
          Serial.println("dispensing!");
          // Открываем слив
          setDispenser(true);
          deadline = millis() + DISPENSER_OPEN_TIME;
          tState = TeaState::cleanning_up;
        }
        else
        {
          Serial.print("no cup! distance: ");
          Serial.println(distance);
        }
      }
    } break;

    case TeaState::cleanning_up: {
      // Ждем пока все закончится
      if (millis() >= deadline)
      {
        Serial.println("done!");
        setDispenser(false);
        stepper_tea.disable();
        stepper_sugar.disable();
        tState = TeaState::idle;
      }
    } break;
  };
}
