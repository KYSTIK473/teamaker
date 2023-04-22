#pragma once

#include <Arduino.h>

#include "TeaParams.h"


class SerialTea
{
public:
  SerialTea(Stream & stream)
    : _stream(stream)
  {}

  // Параметры приготовления чая
  TeaParams params;
  
  // Есть ли активный запрос на приготовление чая
  bool teaRequested = false;

  // нужно вызывать постоянно в loop, чтобы обрабатывать входящие сообщения
  void poll()
  {
    // Принимаем команды вот такого формата:
    // "tea,sugar,heat,water;", где каждое слово это число для соответствующего параметра

    char buffer[50] = {0}; // 50 должно хватить, но нужно бы посчитать
    if (!_stream.available())
      return;
      
    int consumed = _stream.readBytesUntil(';', buffer, sizeof(buffer));
    if (0 == consumed)
      return;

    Serial.print("got line: \"");
    Serial.print(String(buffer));
    Serial.println("\"");

    TeaParams paramsCandidate;
    int argno = 0;
    char * token = strtok(buffer, ",");
    while (token)
    {
      String str(token);
      switch (argno)
      {
        case 0: // tea
          paramsCandidate.teaPortions = str.toInt();
          break;

        case 1: // sugar
          paramsCandidate.sugarPortions = str.toInt();
          break;

        case 2: // heat
          paramsCandidate.heatTemperature = str.toFloat();
          break;

        case 3: // water
          paramsCandidate.waterAmount = str.toFloat();
          break;
      }
      token = strtok(nullptr, ",");
      argno++;
    }

    Serial.print("Params: tea: ");
    Serial.print(paramsCandidate.teaPortions);
    Serial.print(" sugar: ");
    Serial.print(paramsCandidate.sugarPortions);
    Serial.print(" temperature: ");
    Serial.print(paramsCandidate.heatTemperature);
    Serial.print(" water: ");
    Serial.println(paramsCandidate.waterAmount);
    // Если мы не собрали правильное количество аргументов
    if (4 != argno)
    {
      Serial.println("bad arg count");
      return;
    }
    
    // Если переданные параметры не адекватны
    if (!paramsCandidate.isAdequate())
    {
      Serial.println("inadequate params");
      return;
    }

    // Если все ок
    params = paramsCandidate;
    teaRequested = true;
  }

private:
  Stream & _stream;
  
};
