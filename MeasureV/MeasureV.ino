/*
 * Тест INA 219/INA226
 * Выводит в монитор порта платы
 * https://alexgyver.ru/lessons/gyverlibs/
 */
#include <GyverINA.h>   //библа для семейства INA
//#include <INA219.h>   //расскомментировать нужный тип
#include <INA226.h>     //расскомментировать нужный тип

#define SDA 4           //порт SDA на esp8266
#define SCL 5           //порт SCL на esp8266

INA226 ina(0.1, 0.8, 0x40);     // объекта датчика (шунт Ом, макс ток А, адресс)
//INA219 ina(0.1, 3.2, 0x40);
void test_ina();            //функция для теста показаний INA в порте

void setup() {
  // Открываем последовательный порт
  Serial.begin(9600);
  Serial.print(F("INA226..."));
   // Проверяем наличие и инициализируем INA219
  if (ina.begin(D2, D1)) { 
    Serial.println(F("connected!"));
  } else {
    Serial.println(F("not found!"));
    while (1);
  }
}

void loop() {
test_ina ();
}

void test_ina () {
    // Читаем напряжение
  Serial.print(F("Voltage: "));
  Serial.print(ina.getVoltage(), 3);
  Serial.println(F(" V"));

  // Читаем ток
  Serial.print(F("Current: "));
  Serial.print(ina.getCurrent(), 3);
  Serial.println(F(" A"));

  // Читаем мощность
  Serial.print(F("Power: "));
  Serial.print(ina.getPower(), 3);
  Serial.println(F(" W"));

  //читаем напряжение на шунте (не должны быть больше 81.92 мВ)
  Serial.print(F("Shunt voltage: "));
  Serial.print(ina.getShuntVoltage(), 6);
  Serial.println(F(" V"));

  Serial.println("");
  delay(1000);
}
