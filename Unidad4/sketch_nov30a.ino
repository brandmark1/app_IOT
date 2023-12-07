#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include "fauxmoESP.h"


#define DHTPIN 16
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

#define LED_1 35 //ya
#define ID_1 "Baño"
#define LED_2 34//ya
#define ID_2 "Cocina"
#define LED_3 27//ya
#define ID_3 "Ventilador del cuarto principal"
#define LED_4 25//ya
#define ID_4 "Sala"
#define LED_5 14//ya
#define ID_5 "Cuarto principal"
#define LED_6 12 //ya
#define ID_6 "Balcon"
#define LED_7 26 //ya
#define ID_7 "Ventilador de la sala"
#define LED_8 14//ya
#define ID_8 "Cuarto de los niños"
#define LED_9 33//ya
#define ID_9 "Television de la sala"

bool temp, humd = false;

const char *ssid = "R_Sala_de_Computo4";
const char *password = "itcolima6";
const char *mqtt_broker = "v3e23249.ala.us-east-1.emqxsl.com";
const char *topic = "monitores/YG";
const char *mqtt_username = "esp";
const char *mqtt_password = "esp";
const int mqtt_port = 8883;

// load DigiCert Global Root CA ca_cert
const char *ca_cert =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIEqjCCA5KgAwIBAgIQAnmsRYvBskWr+YBTzSybsTANBgkqhkiG9w0BAQsFADBh\n"
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
    "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
    "QTAeFw0xNzExMjcxMjQ2MTBaFw0yNzExMjcxMjQ2MTBaMG4xCzAJBgNVBAYTAlVT\n"
    "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
    "b20xLTArBgNVBAMTJEVuY3J5cHRpb24gRXZlcnl3aGVyZSBEViBUTFMgQ0EgLSBH\n"
    "MTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALPeP6wkab41dyQh6mKc\n"
    "oHqt3jRIxW5MDvf9QyiOR7VfFwK656es0UFiIb74N9pRntzF1UgYzDGu3ppZVMdo\n"
    "lbxhm6dWS9OK/lFehKNT0OYI9aqk6F+U7cA6jxSC+iDBPXwdF4rs3KRyp3aQn6pj\n"
    "pp1yr7IB6Y4zv72Ee/PlZ/6rK6InC6WpK0nPVOYR7n9iDuPe1E4IxUMBH/T33+3h\n"
    "yuH3dvfgiWUOUkjdpMbyxX+XNle5uEIiyBsi4IvbcTCh8ruifCIi5mDXkZrnMT8n\n"
    "wfYCV6v6kDdXkbgGRLKsR4pucbJtbKqIkUGxuZI2t7pfewKRc5nWecvDBZf3+p1M\n"
    "pA8CAwEAAaOCAU8wggFLMB0GA1UdDgQWBBRVdE+yck/1YLpQ0dfmUVyaAYca1zAf\n"
    "BgNVHSMEGDAWgBQD3lA1VtFMu2bwo+IbG8OXsj3RVTAOBgNVHQ8BAf8EBAMCAYYw\n"
    "HQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMBIGA1UdEwEB/wQIMAYBAf8C\n"
    "AQAwNAYIKwYBBQUHAQEEKDAmMCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdp\n"
    "Y2VydC5jb20wQgYDVR0fBDswOTA3oDWgM4YxaHR0cDovL2NybDMuZGlnaWNlcnQu\n"
    "Y29tL0RpZ2lDZXJ0R2xvYmFsUm9vdENBLmNybDBMBgNVHSAERTBDMDcGCWCGSAGG\n"
    "/WwBAjAqMCgGCCsGAQUFBwIBFhxodHRwczovL3d3dy5kaWdpY2VydC5jb20vQ1BT\n"
    "MAgGBmeBDAECATANBgkqhkiG9w0BAQsFAAOCAQEAK3Gp6/aGq7aBZsxf/oQ+TD/B\n"
    "SwW3AU4ETK+GQf2kFzYZkby5SFrHdPomunx2HBzViUchGoofGgg7gHW0W3MlQAXW\n"
    "M0r5LUvStcr82QDWYNPaUy4taCQmyaJ+VB+6wxHstSigOlSNF2a6vg4rgexixeiV\n"
    "4YSB03Yqp2t3TeZHM9ESfkus74nQyW7pRGezj+TC44xCagCQQOzzNmzEAP2SnCrJ\n"
    "sNE2DpRVMnL8J6xBRdjmOsC3N6cQuKuRXbzByVBjCqAA8t1L0I+9wXJerLPyErjy\n"
    "rMKWaBFLmfK/AHNF4ZihwPGOc7w6UHczBZXH5RFzJNnww+WnKuTPI0HfnVH8lg==\n"
    "-----END CERTIFICATE-----\n";
// init secure wifi client
WiFiClientSecure espClient;
// use wifi client to init mqtt client
PubSubClient client(espClient);
fauxmoESP fauxmo;

void wifiSetup() {

    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

}
void setup()
{
  // Set software serial baud to 115200;
  Serial.begin(115200);
  dht.begin();
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  // set root ca cert
  espClient.setCACert(ca_cert);
  // connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected())
  {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
    {
      Serial.println("Public emqx mqtt broker connected");
    }
    else
    {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(client.state());
      Serial.println("Retrying in 5 seconds.");
      delay(5000);
    }
  }
  // publish and subscribe
  client.publish(topic, "Hola EMQX Estoy en ESP32 ");
  client.subscribe(topic);                       

  pinMode(ledPin, OUTPUT);
  pinMode(ascPin, INPUT_PULLUP);
  pinMode(desPin, INPUT_PULLUP);
 // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);

    // LEDs
    pinMode(LED_1, OUTPUT);
    digitalWrite(LED_1, LOW);
    pinMode(LED_2, OUTPUT);
    digitalWrite(LED_2, LOW);
    pinMode(LED_3, OUTPUT);
    digitalWrite(LED_3, LOW);
    pinMode(LED_4, OUTPUT);
    digitalWrite(LED_4, LOW);
    pinMode(LED_5, OUTPUT);
    digitalWrite(LED_5, LOW);
    pinMode(LED_6, OUTPUT);
    digitalWrite(LED_6, LOW);
    pinMode(LED_7, OUTPUT);
    digitalWrite(LED_7, LOW);
    pinMode(LED_8, OUTPUT);
    digitalWrite(LED_8, LOW);
    pinMode(LED_9, OUTPUT);
    digitalWrite(LED_9, LOW);
    
    // Wifi
    wifiSetup();
    
    fauxmo.createServer(true); // not needed, this is the default value
    fauxmo.setPort(80); // This is required for gen3 devices
    fauxmo.enable(true);
    // Add virtual devices
    fauxmo.addDevice(ID_1);
    fauxmo.addDevice(ID_2);
    fauxmo.addDevice(ID_3);
    fauxmo.addDevice(ID_4);
    fauxmo.addDevice(ID_5);
    fauxmo.addDevice(ID_6);
    fauxmo.addDevice(ID_7);
    fauxmo.addDevice(ID_8);
    fauxmo.addDevice(ID_9);        
    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
        
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);

        if (strcmp(device_name, ID_1)==0) {
            digitalWrite(LED_1, state ? HIGH : LOW);
        }else if (strcmp(device_name, ID_2)==0) {
            digitalWrite(LED_2, state ? HIGH : LOW);
        }else if (strcmp(device_name, ID_3)==0) {
            digitalWrite(LED_3, state ? HIGH : LOW);
        }else if (strcmp(device_name, ID_4)==0) {
            digitalWrite(LED_4, state ? HIGH : LOW);
        }else if (strcmp(device_name, ID_5)==0) {
            digitalWrite(LED_5, state ? HIGH : LOW);
        }else if (strcmp(device_name, ID_6)==0) {
            digitalWrite(LED_6, state ? HIGH : LOW);
        }else if (strcmp(device_name, ID_7)==0) {
            digitalWrite(LED_7, state ? HIGH : LOW);
        }else if (strcmp(device_name, ID_8)==0) {
            digitalWrite(LED_8, state ? HIGH : LOW);
        }else if (strcmp(device_name, ID_9)==0) {
            digitalWrite(LED_9, state ? HIGH : LOW);
        }

    });

}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();


  DynamicJsonDocument jsonDocument(1024);


  DeserializationError error = deserializeJson(jsonDocument, payload, length);

  if (error)
  {
    Serial.print("JSON parsing failed: ");
    Serial.println(error.c_str());
    return;
  }


  if (jsonDocument.containsKey("cocina"))
  {
    int ledValue = jsonDocument["cocina"].as<int>();


    if (ledValue == 1)
    {

      digitalWrite(LED_2, HIGH);
      Serial.println("cocina encendido");
    }
    else if (ledValue == 0)
    {

      digitalWrite(LED_2, LOW);
      Serial.println("cocina apagado");
    }
    else
    {
      Serial.println("JSON no contiene la clave 'cocina' o 'cocina' no es un número.");
    }
  }

  if (jsonDocument.containsKey("sala"))
  {
    int ledValue = jsonDocument["sala"].as<int>();


    if (ledValue == 1)
    {

      digitalWrite(LED_4, HIGH);
      Serial.println("sala encendido");
    }
    else if (ledValue == 0)
    {

      digitalWrite(LED_4, LOW);
      Serial.println("sala apagado");
    }
    else
    {
      Serial.println("JSON no contiene la clave 'sala' o 'sala' no es un número.");
    }
  }

    if (jsonDocument.containsKey("recamara"))
  {
    int ledValue = jsonDocument["recamara"].as<int>();


    if (ledValue == 1)
    {

      digitalWrite(LED_5, HIGH);
      Serial.println("Cuarto principal encendido");
    }
    else if (ledValue == 0)
    {

      digitalWrite(LED_5, LOW);
      Serial.println("Cuarto principal apagado");
    }
    else
    {
      Serial.println("JSON no contiene la clave 'recamara principal' o 'recamara principal' no es un número.");
    }
  }

      if (jsonDocument.containsKey("bano"))
  {
    int ledValue = jsonDocument["bano"].as<int>();


    if (ledValue == 1)
    {

      digitalWrite(LED_1, HIGH);
      Serial.println("baño encendido");
    }
    else if (ledValue == 0)
    {

      digitalWrite(LED_1, LOW);
      Serial.println("baño apagado");
    }
    else
    {
      Serial.println("JSON no contiene la clave 'baño' o 'baño' no es un número.");
    }
  }

        if (jsonDocument.containsKey("terraza"))
  {
    int ledValue = jsonDocument["terraza"].as<int>();


    if (ledValue == 1)
    {

      digitalWrite(LED_6, HIGH);
      Serial.println("Balcon encendido");
    }
    else if (ledValue == 0)
    {

      digitalWrite(LED_6, LOW);
      Serial.println("Balcon apagado");
    }
    else
    {
      Serial.println("JSON no contiene la clave 'terraza' o 'terraza' no es un número.");
    }
  }

          if (jsonDocument.containsKey("cuarto"))
  {
    int ledValue = jsonDocument["cuarto"].as<int>();


    if (ledValue == 1)
    {

      digitalWrite(LED_8, HIGH);
      Serial.println("cuarto de niño encendido");
    }
    else if (ledValue == 0)
    {

      digitalWrite(LED_8, LOW);
      Serial.println("cuarto de niño apagado");
    }
    else
    {
      Serial.println("JSON no contiene la clave 'cuarto de niño' o 'cuarto de niño' no es un número.");
    }
  }

          if (jsonDocument.containsKey("ventiladorsala"))
  {
    int ledValue = jsonDocument["ventiladorsala"].as<int>();


    if (ledValue == 1)
    {

      digitalWrite(LED_7, HIGH);
      Serial.println("ventilador sala encendido");
    }
    else if (ledValue == 0)
    {

      digitalWrite(LED_7, LOW);
      Serial.println("ventilador sala apagado");
    }
    else
    {
      Serial.println("JSON no contiene la clave 'ventiladorsala' o 'ventiladorsala' no es un número.");
    }
  }

        if (jsonDocument.containsKey("ventiladorprincipal"))
  {
    int ledValue = jsonDocument["ventiladorprincipal"].as<int>();


    if (ledValue == 1)
    {

      digitalWrite(LED_3, HIGH);
      Serial.println("Ventilador del cuarto principal encendido");
    }
    else if (ledValue == 0)
    {

      digitalWrite(LED_3, LOW);
      Serial.println("Ventilador del cuarto principal apagado");
    }
    else
    {
      Serial.println("JSON no contiene la clave 'Ventilador del cuarto principal' o 'Ventilador del cuarto principal' no es un número.");
    }
  }

          if (jsonDocument.containsKey("television"))
  {
    int ledValue = jsonDocument["television"].as<int>();


    if (ledValue == 1)
    {

      digitalWrite(LED_9, HIGH);
      Serial.println("Television de la sala encendido");
    }
    else if (ledValue == 0)
    {

      digitalWrite(LED_9, LOW);
      Serial.println("Television de la sala apagado");
    }
    else
    {
      Serial.println("JSON no contiene la clave 'Television de la sala' o 'Television de la sala' no es un número.");
    }
  }
   if (jsonDocument.containsKey("temperature"))
   {
      Serial.println("Temperature");

      temp = true;
   }

      if (jsonDocument.containsKey("humidity"))
   {
      Serial.println("Humidity");
      humd = true;
   }

  Serial.println("....................");
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.println("Reconnecting to MQTT broker...");
    String client_id = "esp8266-client-";
    client_id += String(WiFi.macAddress());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
    {
      Serial.println("Reconnected to MQTT broker.");
      client.subscribe(topic);
    }
    else
    {
      Serial.print("Failed to reconnect to MQTT broker, rc=");
      Serial.print(client.state());
      Serial.println("Retrying in 5 seconds.");
      delay(5000);
    }
  }
}
unsigned long UltimatempHumEnviadatiempo = 0;
const unsigned long tempHumEnviarIntervalo = 30000; 

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  if(digitalRead(ascPin) == HIGH)
  {
      counter++;
      // Publica el nuevo valor del contador en el tema MQTT
      DynamicJsonDocument jsonDoc(128);
      jsonDoc["from"] = "ESP32";
      jsonDoc["to"] = "broadcast";
      jsonDoc["action"] = "UPDATE_COUNTER";
      jsonDoc["value"] = counter;

      char buffer[128];
      serializeJson(jsonDoc, buffer);
      client.publish(topic, buffer);
      delay(300);
      Serial.println("Asc");
  }
  if(digitalRead(desPin) == HIGH)
  {
      counter--;
      // Publica el nuevo valor del contador en el tema MQTT
      DynamicJsonDocument jsonDoc(128);
      jsonDoc["from"] = "ESP32";
      jsonDoc["to"] = "broadcast";
      jsonDoc["action"] = "UPDATE_COUNTER";
      jsonDoc["value"] = counter;

      char buffer[128];
      serializeJson(jsonDoc, buffer);
      client.publish(topic, buffer);
      delay(300);
      Serial.println("Dec");
  }

unsigned long concurenciadetiempo = millis();
  if(concurenciadetiempo - UltimatempHumEnviadatiempo >= tempHumEnviarIntervalo) 
  {
      float temperature = dht.readTemperature();
      float humidity = dht.readHumidity();
      if (!isnan(temperature) && !isnan(humidity))
      {
        DynamicJsonDocument jsonDoc(128);
          jsonDoc["from"] = "ESP32";
          jsonDoc["to"] = "server";
          jsonDoc["action"] = "SEND_DATA";
          JsonObject data = jsonDoc.createNestedObject("data");
          data["temperature"] = temperature;
          data["humidity"] = humidity;

          char buffer[200];
          serializeJson(jsonDoc, buffer);
          client.publish(topic, buffer);
          jsonDoc["to"] = "web";
          serializeJson(jsonDoc, buffer);
          client.publish(topic, buffer);
      }

      UltimatempHumEnviadatiempo = concurenciadetiempo;
  }
    fauxmo.handle();
}
