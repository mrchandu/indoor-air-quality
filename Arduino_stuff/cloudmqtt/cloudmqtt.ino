#include <ESP8266WiFi.h>
#include <PubSubClient.h>   // Read the rest of the article
#include "MQ135.h"
#include <stdlib.h>



const char *ssid =  "Srinivasa 2nd floor";   // cannot be longer than 32 characters!
const char *pass =  "9036702497";   //
const char *mqtt_server = "m14.cloudmqtt.com";
const int mqtt_port = 19786;
const char *mqtt_user = "pserzwxn";
const char *mqtt_pass = "JwgICBTk7hdL";

#define BUFFER_SIZE 100

unsigned long previousMillis = 0;
const long interval = 10000;

int sensor = A0;
int sensorValue = 0;

MQ135 gasSensor = MQ135(sensor);  

WiFiClient wclient;
PubSubClient client(wclient, mqtt_server,  mqtt_port);

void callback(const MQTT::Publish& pub) {
Serial.print(pub.topic());
Serial.print(" => ");
 if (pub.has_stream()) {
    uint8_t buf[BUFFER_SIZE];
    int read;
    while (read = pub.payload_stream()->read(buf, BUFFER_SIZE)) {
      Serial.write(buf, read);
    }
    pub.payload_stream()->stop();
 
    if(buf[0]=='-' && buf[1]=='1'){
       digitalWrite(LED_BUILTIN, HIGH);
       delay(1000);
       digitalWrite(LED_BUILTIN, LOW);  
    }
     
    Serial.println("");
  }
Serial.println(pub.payload_string());
if(pub.payload_string()=="on"){
 digitalWrite(LED_BUILTIN, LOW);
 delay(4000);
Serial.println("led on");
}
if(pub.payload_string()=="off"){
 digitalWrite(LED_BUILTIN, HIGH);
 delay(4000);
Serial.println("led off");
}
}

void setup() {
Serial.begin(115200); 
delay(10);
pinMode(LED_BUILTIN, OUTPUT); 
 digitalWrite(LED_BUILTIN, LOW);
  delay(2000); 
  digitalWrite(LED_BUILTIN, HIGH);
Serial.println();
Serial.println();
}
void loop() {
if (WiFi.status() != WL_CONNECTED) { 
Serial.print("Connecting to ");
Serial.print(ssid);
Serial.println("...");
WiFi.begin(ssid, pass);
if (WiFi.waitForConnectResult() != WL_CONNECTED)
return;
Serial.println("WiFi connected");
}
if (WiFi.status() == WL_CONNECTED) {

if (!client.connected()) {
Serial.println("Connecting to MQTT server");

if (client.connect(MQTT::Connect("mqtt_client_name")
.set_auth(mqtt_user, mqtt_pass))) {
Serial.println("Connected to MQTT server");

client.set_callback(callback);
client.subscribe("sensor");
} else {
Serial.println("Could not connect to MQTT server");   
}
}
if (client.connected())
client.loop();
}
Sendppmvalue();
}
void Sendppmvalue(){

  sensorValue = analogRead(sensor);
  Serial.println(sensorValue, DEC);
  float rzero = gasSensor.getRZero();
  float ppm = gasSensor.getPPM();
  //Serial.println(rzero);
  //Serial.println(ppm);
  Serial.print("ppm"); 
  Serial.println(ppm);
  delay(1000);
client.publish("ppm",String(ppm) );
}
