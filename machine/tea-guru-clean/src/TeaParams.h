#pragma once


struct TeaParams
{
  // Количество порций чая
  int teaPortions = 1;

  // Количество порций сахара
  int sugarPortions = 1;

  // Температура нагрева воды
  float heatTemperature = 96.0;

  // Наливаемое количество воды (в миллилитрах)
  float waterAmount = 200.0f;

  // Проверка на адекватность
  bool isAdequate() const
  {
    if (teaPortions <= 0 || teaPortions > 4) return false;
    if (sugarPortions < 0 || sugarPortions > 4) return false;
    if (heatTemperature > 100.0) return false;
    if (waterAmount <= 0 || waterAmount >= 700) return false; // TODO: исправить на минимум, чтобы накрыть тэн

    return true;
  }
};


