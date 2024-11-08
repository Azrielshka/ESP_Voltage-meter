/*
 * Тестовая програ для интерфейса ESP8266. Выводит просто цифры в приложение. Тест интерфейса
 * Хост: test.mosquitto.org
 * Порт (ESP): 1883
 * Порт (приложение): 8081
 * имя сети - это типо пароль. нужно оригинал - чтобы не нашли в сети
 */
#include <Arduino.h>
//настройки роутера
#define Wifi_log "RT-WiFi-03FD"
#define Wifi_pass "bPXtFtGX"

// отключаем встроенную реализацию MQTT (для esp)
#define GH_NO_MQTT

#include <GyverHub.h>
GyverHub hub("Azrielshka_Dev", "Pathfinder", "f1eb");  // имя сети, имя устройства, иконка

#include <PairsFile.h>
#include <Pairs.h>
#include <PairsExt.h>
#include <PairsStatic.h>
Pairs p; //база пар для вывода на гайверовских pairs

// MQTT
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
 
//переменные
double volt = 0.01;
double power = 0.02;
double cur = 0.03;
String norm = "OK";
String bad = "BaD";

//имена ключей в методах pairs
String volt_S = "Напряжение ";
String amper_S = "Ток ";
String power_S = "Мощность ";
String sost = "Состояние";

//настройки моста, НЕ трогать, хз как работает
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
   b.Title("Common");
   {
    gh::Row r(b);
    b.Text_("name_text",p);
    b.widget.label("Показания");
    b.widget.rows(4);
    b.widget.size(2, 2);
    }
}
//билдер графики
void build_gauge (gh::Builder& b) {
  {
    gh::Row r(b);
    b.GaugeLinear_("gagl").size(3);
    b.widget.value(p[volt]);
    b.widget.range(0,100,5);
  }
}
// билдер общий
void build(gh::Builder& b) {
   build_pairs(b);
   build_gauge(b);
}

void setup() {
    Serial.begin(115200);
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
    p[volt_S] = volt+=0.01;
    p[amper_S] = cur+=0.01;
    p[power_S] = power+=0.01;
    p[sost] = norm;
    hub.sendUpdate("name_text", p);
    hub.sendUpdate("gagl", p[volt_S]);
    }
}
