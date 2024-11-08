# ESP_Voltage-meter
IOT power. V 1.0. Отслеживание уровня заряда аккумов 7 / 12 / 24 В на ESP8266/ESP32 на INA и MQTT, работает с датчиками INA219, INA226

Устройство измеряет напряжение на аккумуляторе, преобразует в процент заряда и отправляет на сервер MQTT. Мониторинг можно производить в приложении GyverHub (доступно AppStore, GooglePlay)

<img src="https://github.com/user-attachments/assets/d01430d7-fd4d-4cf9-a829-d07ced1adbdd" 
width=30% height=30%>
<img src="https://github.com/user-attachments/assets/67adf0f7-ece6-4531-a142-2989cad0c537" 
width=50% height=50%>

Файлы проекта содержат stl модели для корпуса устройства выполненного на распаечной плате размером 70х50 мм

* Компоненты ESP_Voltage-meter:
	* 1. Датчик тока INA
   	* 2. ESP8266/ESP32 (нетестировалось)
   	* 3. Распаечная плата 70х50 мм
   	* 4. PIN PCB connector mother/father



![IMG_7396](https://github.com/user-attachments/assets/1521381b-db4c-438b-a157-75bdb11d0623)

Тестировалось с INA226, относительная погрешность около 0.1 %

![IMG_7401](https://github.com/user-attachments/assets/59c09095-dd76-4e89-a066-4f06377baf55)

![photo_2024-11-08_15-56-29](https://github.com/user-attachments/assets/91c81461-6a15-47e3-9b62-b30f59fb3db0)


Моделирование корпуса для ESP_Voltage-meter выполненно в КОМПАС-3D v.21

![IMG_7675](https://github.com/user-attachments/assets/4396c432-38e3-4665-89e7-814c86ddc8b1)


Основной файл прошивки ESP8266 - MesFull_V1.0.

	* Отлажено на хосте "Москито"
	* Хост: test.mosquitto.org
	* Порт(ESP) : 1883
	* Порт(приложение) : 8081
	* имя сети - это типо пароль. нужно оригинал - чтобы не нашли в сети

Инструкция по использованию:

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


