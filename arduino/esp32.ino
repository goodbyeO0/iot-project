#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>
#include <SPI.h>
#include <MFRC522.h>
#define enB 27
#define in3 26
#define in4 25
#define RST_PIN 22 // Configurable, see typical pin layout above
#define SS_PIN 21  // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

DHT dht(14, DHT11);

int powerPin = 13;
int kipasSpeed = 0;

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

    Serial.println("Connected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    dht.begin();
    server.begin();
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522
    Serial.println("Approximate your card to the reader...");
    Serial.println();
}

void loop()
{
    HTTPClient httpGet;            // Declare httpGet at the start of loop
    DynamicJsonDocument doc(1024); // Declare doc at the start of loop

    // Look for new cards
    if (!mfrc522.PICC_IsNewCardPresent())
    {
        return;
    }

    // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial())
    {
        return;
    }

    // Capture UID in a single variable
    String uidStr = "";
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        uidStr += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "") + String(mfrc522.uid.uidByte[i], HEX);
    }

    // Print the UID
    Serial.print("Card UID: ");
    Serial.println(uidStr);

    digitalWrite(powerPin, HIGH);
    float kelembaban = dht.readHumidity();
    float suhu = dht.readTemperature();
    Serial.print("kelembaban: ");
    Serial.print(kelembaban);
    Serial.print(" ");
    Serial.print("suhu: ");
    Serial.println(suhu);

    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient httpPost;
        DynamicJsonDocument doc(1024);
        String requestBody;
        int httpCodePost;

        httpPost.begin("http://192.168.178.208:3002/postUID");
        httpPost.addHeader("Content-Type", "application/json");

        doc["UID"] = uidStr;
        serializeJson(doc, requestBody);
        Serial.println("Sending UID to server...");
        Serial.println(requestBody); // Debug print

        httpCodePost = httpPost.POST(requestBody);
        Serial.print("Status code: ");
        Serial.println(httpCodePost); // Debug print

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

        httpPost.end(); // Ensure the connection is closed properly
    }

    // Send GET request
    httpGet.begin("https://animated-choux-c90517.netlify.app/.netlify/functions/api/getKipas");
    int httpCodeGet = httpGet.GET();

    if (httpCodeGet > 0)
    {
        String payloadGet = httpGet.getString();
        Serial.println(payloadGet.c_str());

        deserializeJson(doc, payloadGet);
        kipasSpeed = doc["kelajuan"]; // Assuming 'kelajuan' is the correct key for fan speed
        Serial.println(kipasSpeed);
    }
    else
    {
        Serial.print("GET request failed with status code ");
        Serial.println(httpCodeGet);
    }

    httpGet.end();

    Serial.println(kipasSpeed);   // Print the value of motorSpeedA
    analogWrite(enB, kipasSpeed); // Send PWM signal to motor A
    // Set Motor A backward
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    delay(3000);
}