#pragma once


#include <Arduino.h>
#include <OneWire.h>
#include <GyverButton.h>

#include "CustomStepper.h"
#include "pcf8574.h"

#include "SerialTea.h"


// Время работы помпы для налива холодной воды в мс для одной кружки
#define COLD_WATER_POUR_TIME (5000)

#define PIN_DS A0
#define PIN_TEN 3
#define PIN_KRAN 2
#define PIN_HOLOD 5
#define PIN_KIP 6

#define PIN_TRIG 8
#define PIN_ECHO 9


// Сколько времени крутить холодную помпу в мс на миллитр воды
#define COLD_PUMP_MS_PER_ML (1000/12.f)
// Сколько времени крутить горячую помпу в мс на миллилитр воды
#define HOT_PUMP_MS_PER_ML  (COLD_PUMP_MS_PER_ML/1.f)
// Сколько мс максимум можно держать кипятильник включенным (в мс)
#define HEATER_MAX_TIME (120*1000UL) // (10*1000UL)
// Максимальная температура кипятильника в градусах цельсия
//#define MAX_HEATER_TEMP (90) // TODO
// Сколько времени чай нужно заваривать (в мс)
#define BREW_TIME (10*1000UL); //(60*1000UL) // (10*1000UL)
// Время, сколько мы держим кран открытым (в мс)
#define DISPENSER_OPEN_TIME (15*1000UL)


extern OneWire ds;
extern PCF8574 pcf;
extern CustomStepper stepper_tea;
extern CustomStepper stepper_sugar;
extern GButton butt1;
extern SerialTea serialTea;


// Настойка железа пинов и прочего
void setupHardware();

// Получение температуры
float getBoilerTemp();

// замер расстояния до кружки
int cupDistance();


// Управление кипятильником
void setBoilder(bool enabled);
// Управление холодным насосом
void setColdPump(bool enabled);
// Управление горячим насосом
void setHotPump(bool enabled);
// Запуск шаговика для чая (на указанное количество порций)
void startTeaStepper(int portions);
// Запуск шаговика для сахара (на указааное количество порцией)
void startSugarStepper(int portions);

// Управление сливным клапаном
void setDispenser(bool opened);
