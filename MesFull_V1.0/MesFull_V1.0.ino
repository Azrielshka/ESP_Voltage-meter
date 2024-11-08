/*
 * IOT power. V 1.0. Отслеживание уровня заряда аккумов ESP8266/ESP32 на INA и MQTT
 * Все настройки в заголовочном файле Azrielshka_Header1.h
 * https://github.com/GyverLibs/GyverHub?tab=readme-ov-file - главный источник
 * https://github.com/GyverLibs/GyverHub/blob/main/docs/3.app.md - приложение
 * https://github.com/GyverLibs/GyverHub/wiki - документация
 * by. Azrielshka
 */
 
//заголовочник, все настройки там 
#include "Azrielshka_Header1.h"

// отключаем встроенную реализацию MQTT (для esp)
#define GH_NO_MQTT

#include <GyverHub.h>
GyverHub hub(NAME_NETWORK, NAME_DEVICE, LOGO);  // имя сети, имя устройства, иконка. Настраивается в Azrielshka_Header1.h

//#include <PairsFile.h>
#include <Pairs.h>
#include <PairsExt.h>
#include <PairsStatic.h>
Pairs p; //база пар для вывода на гайверовских pairs

// MQTT
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//INA в зависимости от настройки VERSION_INA в заголовочнике
#include <GyverINA.h>   //библа для семейства INA
#if (VERSION_INA == 0)
#include <INA219.h>
INA219 ina(Shunt, Max_tok, Adress);       // объекта датчика (шунт Ом, макс ток А, адресс) 
#else
#include <INA226.h>     //расскомментировать нужный тип
INA226 ina(Shunt, Max_tok, Adress);     // объекта датчика (шунт Ом, макс ток А, адресс)
#endif

#define SDA D2           //порт SDA на esp8266
#define SCL D1           //порт SCL на esp8266 

//переменные
float volt = 0.01;       // хранение показаний Напряжения, В
float power = 0.02;      // хранение Мощности, Вт
float cur = 0.03;        // хранение показаний Силы тока, А
String norm = "OK";      // состояние аккумов
String bad = "Разряжен";      // состояние аккумов

//имена ключей в методах pairs
String volt_S = "Напряжение, В ";
String amper_S = "Ток ";
String power_S = "Мощность ";
String sost = "Состояние ";

//настройки моста, НЕ трогать, хз как работает. Взято из примера бибилотеки по настройке MQTT
class HubMQTT : public gh::Bridge {
   public:
    HubMQTT(void* hub) : gh::Bridge(hub, gh::Connection::MQTT, GyverHub::parseHook), mqtt(espClient) {}

    void begin() {
        mqtt.setServer("test.mosquitto.org", 1883);
        mqtt.setCallback([this](char* topic, uint8_t* data, uint16_t len) {
            uint16_t tlen = strlen(topic);
            char topic_buf[tlen + 1];
            memcpy(topic_buf, topic, tlen);
            topic_buf[tlen] = 0;

            char data_buf[len + 1];
            memcpy(data_buf, data, len);
            data_buf[len] = 0;

            parse(sutil::AnyText(topic_buf, tlen), sutil::AnyText(data_buf, len));
        });
    }
    void end() {
        mqtt.disconnect();
    }
    void tick() {
        if (!mqtt.connected()) reconnect();
        mqtt.loop();
    }
    void send(gh::BridgeData& data) {
        if (!mqtt.connected()) return;
        mqtt.beginPublish(data.topic.c_str(), data.text.length(), 0);
        mqtt.print(data.text);
        mqtt.endPublish();
    }

   private:
    WiFiClient espClient;
    PubSubClient mqtt;

    void reconnect() {
        while (!mqtt.connected()) {
            String clientId = "hub-";
            clientId += String(random(0xffff), HEX);
            if (mqtt.connect(clientId.c_str())) {
                Serial.println("connected");
                mqtt.subscribe(hub.topicDiscover().c_str());
                mqtt.subscribe(hub.topicHub().c_str());
            } else {
                delay(1000);
            }
        }
    }
};

// МQTT объект
HubMQTT mqtt(&hub);

//билдер показаний
void build_pairs(gh::Builder& b) {
   b.Title("Акуммуляторы");
   {
    gh::Row r(b);
    b.Text_("name_text",p);
    b.widget.label("Показания");
    b.widget.fontSize(25);
    b.widget.rows(4);
    //b.widget.size(3, 2);
    
    }
}
//билдер графики
void build_gauge (gh::Builder& b) {
  {
    gh::Row r(b);
    b.GaugeLinear_("gagl").size(3);
    b.widget.value(p[volt]);
    b.widget.range(0,100,2);
  }
}
// билдер общий
void build(gh::Builder& b) {
   build_pairs(b);
   build_gauge(b);
}

void setup() {
    Serial.begin(115200);
    //ПОДКЛЮЧАЕМ INA
    Serial.print(F("INA..."));
    // Проверяем наличие и инициализируем INA219
    if (ina.begin(SDA, SCL)) { 
      Serial.println(F("connected!"));
    } else {
      Serial.println(F("not found!"));
      //while (1);
    }
    // подключение к WiFi..
    WiFi.mode(WIFI_STA);
    WiFi.begin(Wifi_log, Wifi_pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println(WiFi.localIP());
    hub.onBuild(build);   // подключаем билдер
    hub.addBridge(&mqtt); // добавляем мост
    hub.begin();          // запускаем систему
}

void loop() {
    hub.tick();   // тикаем тут
    static gh::Timer tmr(1000);  // период обновления показанний в приложении - 1 секунда (опционально)
    if (tmr) {
    //читаем показания
    p[volt_S] = ina.getVoltage();
    //p[amper_S] = ina.getCurrent();
    //p[power_S] = ina.getPower();
    float vf = p[volt_S];
    if (vf > min_volt) {
      p[sost] = norm;
    }
    else {
      p[sost] = bad;
    }
    //заполняем шкалу с процентами
    int a = trunc((vf - min_volt) / (max_volt - min_volt) * 100);
    //проверка на положительное значение. Например, если vf = 18, а минимальное пороговое - 20, тогда 18-20 = -2.
    a = a > 0 ? a : 0;
    hub.sendUpdate("name_text", p);
    hub.sendUpdate("gagl", a);
    }
}
