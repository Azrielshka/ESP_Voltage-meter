# ESP_Voltage-meter
IOT power. V 1.0. Отслеживание уровня заряда аккумов ESP8266/ESP32 на INA и MQTT

Основной файл прошивки ESP8266 - MesFull_V1.0.

1.Для корректной работы нужно установить настройки в заголовочном файле
\ESP_Voltage meter\firmware\MesFull_V1.0\Azrielshka_Header1.h

2.Определить номинал сопротивления шунта на датчике INA и определить максимальный ток.
Записать их в заголовочный файл Azrielshka_Header1.h.

3. Задать имя и пароль сети wifi в заголовочном файле Azrielshka_Header1.h.
Строки Wifi_log и Wifi_pass.

4. Изменить имя устройства на MQTT сервер в в заголовочном файле Azrielshka_Header1.h.
NAME_NETWORK и NAME_DEVICE

5.Установить библиотеки из папки libs в папку Arduino с библиотеками.
\Documents\Arduino\libraries
Без этого прошивка загружаться в плату не будет!

6. В среде ArduinoIDE во вкладке с настройками вставить ссылку для менеджера плат
https://arduino.esp8266.com/stable/package_esp8266com_index.json 

7. Установить менеджер плат по поиску ESP8266

8. Скачать с AppStore/GooglePlay приложение GyverHub

9. Установить настройки MQTT сервера:
test.mosquitto.org
8081

10. Установить настройки. Имя сети из пункта 4 - NAME_NETWORK.
Префикс: NAME_NETWORK


