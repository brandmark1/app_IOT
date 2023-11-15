#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <DHT.h>
#include <ArduinoJson.h>

const char *ssid = "W_Aula_WB11";
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

WiFiClientSecure espClient;
PubSubClient client(espClient);

const int ledPin = 33; // Pin del LED
int ledState = LOW;
int temperature = 0;
int humidity = 0;
int counter = 0;

DHT dht(26, DHT11);
const int BOTON_MAS_PIN = 32; // Pin para el botón de incremento (conéctalo al pin GPIO correspondiente)
const int BOTON_MENOS_PIN = 35; // Pin para el botón de decremento (conéctalo al pin GPIO correspondiente)

void setup()
{
  Serial.begin(115200);
  setup_wifi();
  espClient.setCACert(ca_cert);
  // connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  dht.begin();
  pinMode(ledPin, OUTPUT);
  pinMode(BOTON_MAS_PIN, INPUT_PULLUP); // Configura el botón de incremento como entrada con resistencia pull-up
  pinMode(BOTON_MENOS_PIN, INPUT_PULLUP); // Configura el botón de decremento como entrada con resistencia pull-up
}

void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Conectado a la red WiFi");
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Conectando al servidor MQTT...");
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
    {
      Serial.println("Conectado");
      client.subscribe(topic);
    }
    else
    {
      Serial.print("Error de conexión, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

void incrementCounter()
{
  counter++;
  // Publica el nuevo valor del contador en el tema MQTT
  DynamicJsonDocument jsonDoc(128);
  jsonDoc["from"] = "ESP32";
  jsonDoc["to"] = "broadcast";
  jsonDoc["action"] = "UPDATE_COUNTER";
  jsonDoc["value"] = counter;

  char jsonStr[128];
  serializeJson(jsonDoc, jsonStr);
  client.publish(topic, jsonStr);
}

void decrementCounter()
{
  counter--;
  // Publica el nuevo valor del contador en el tema MQTT
  DynamicJsonDocument jsonDoc(128);
  jsonDoc["from"] = "ESP32";
  jsonDoc["to"] = "broadcast";
  jsonDoc["action"] = "UPDATE_COUNTER";
  jsonDoc["value"] = counter;

  char jsonStr[128];
  serializeJson(jsonDoc, jsonStr);
  client.publish(topic, jsonStr);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido en el tópico: ");
  Serial.println(topic);

  // Crear un buffer y copiar el payload recibido en él
  char buffer[128];
  memcpy(buffer, payload, length);
  buffer[length] = '\0'; // Asegurar que el buffer es una cadena de caracteres válida

  // Controlar el LED con el mensaje JSON
  controlarLED(buffer);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  if (digitalRead(BOTON_MAS_PIN) == LOW)
  {
    incrementCounter();
  }

  if (digitalRead(BOTON_MENOS_PIN) == LOW)
  {
    decrementCounter();
  }

  DynamicJsonDocument jsonDoc(128);
  jsonDoc["from"] = "ESP32";
  jsonDoc["to"] = "server";
  jsonDoc["action"] = "SEND_DATA";
  JsonObject data = jsonDoc.createNestedObject("data");
  data["temperature"] = temperature;
  data["humidity"] = humidity;

  char jsonStr[128];
  serializeJson(jsonDoc, jsonStr);
  client.publish(topic, jsonStr);

  digitalWrite(ledPin, ledState);

  delay(1000);
}

void controlarLED(const char *payload)
{
  // Parsear el JSON recibido
  DynamicJsonDocument jsonDoc(128);
  DeserializationError error = deserializeJson(jsonDoc, payload);

  // Verificar errores de parseo
  if (error)
  {
    Serial.print("Error al parsear JSON: ");
    Serial.println(error.c_str());
    return;
  }

  // Verificar si el JSON tiene la clave "led" y es un número
  if (jsonDoc.containsKey("led") && jsonDoc["led"].is<int>())
  {
    int estadoLED = jsonDoc["led"].as<int>();

    // Ajustar el estado del LED
    if (estadoLED == 1)
    {
      ledState = HIGH; // Encender el LED
    }
    else if (estadoLED == 0)
    {
      ledState = LOW; // Apagar el LED
    }

    digitalWrite(ledPin, ledState);
    Serial.print("Estado del LED actualizado a: ");
    Serial.println(ledState);
  }
  else
  {
    Serial.println("JSON no contiene la clave 'led' o 'led' no es un número.");
  }
}