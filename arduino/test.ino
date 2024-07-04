#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>
#define enB 27
#define in3 26
#define in4 25

DHT dht(14, DHT11);

int powerPin = 12;

const char *ssid = "afik uitm";
const char *password = "afikjasin";

AsyncWebServer server(80);
char data[50]; // Use char array instead of String

void setup()
{
    pinMode(enB, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    pinMode(13, OUTPUT);
    pinMode(powerPin, OUTPUT);

    digitalWrite(powerPin, LOW);

    Serial.begin(9600);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    delay(5000); // Add delay after WiFi connection
    Serial.println("Connected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    dht.begin();
    server.begin();
}

void loop()
{

    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient httpPost;

        // Send POST request
        httpPost.begin("http://192.168.238.208:3000/postSuhu"); // Specify the URL
        httpPost.addHeader("Content-Type", "application/json"); // Specify the content type

        // Create the JSON object
        DynamicJsonDocument doc(1024);
        doc["suhu"] = suhu;
        doc["kelembapan"] = kelembaban;
        String requestBody;
        serializeJson(doc, requestBody);

        int httpCodePost = httpPost.POST(requestBody); // Send the POST request

        if (httpCodePost > 0)
        {
            String payloadPost = httpPost.getString();
            Serial.println(payloadPost);
        }

        else
        {
            Serial.print("POST request failed with status code ");
            Serial.println(httpCodePost);
        }

        httpPost.end();

        // Send GET request
        HTTPClient httpGet;
        httpGet.begin("http://192.168.238.208:3000/postSuhu/getKipas");
        int httpCodeGet = httpGet.GET();

        if (httpCodeGet > 0)
        {
            String payloadGet = httpGet.getString();
            Serial.println(payloadGet.c_str());

            DynamicJsonDocument doc(1024);
            deserializeJson(doc, payloadGet);

            int value = doc["suhu"];
            snprintf(data, sizeof(data), "%d", value); // Convert integer to string and copy it to data

            Serial.println(data);
        }

        else
        {
            Serial.print("GET request failed with status code ");
            Serial.println(httpCodeGet);
        }

        httpGet.end();
    }

    int motorSpeedB = 255;
    Serial.println(motorSpeedB);   // Print the value of motorSpeedA
    analogWrite(enB, motorSpeedB); // Send PWM signal to motor A
    // Set Motor A backward
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    delay(1000);

    digitalWrite(powerPin, HIGH);
    float kelembaban = dht.readHumidity();
    float suhu = dht.readTemperature();
    Serial.print("kelembaban: ");
    Serial.print(kelembaban);
    Serial.print(" ");
    Serial.print("suhu: ");
    Serial.println(suhu);

    delay(3000);
}