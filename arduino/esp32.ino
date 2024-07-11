#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>
#include <SPI.h>
#include <MFRC522.h>
#define enA 27
#define in2 26
#define in1 25
#define RST_PIN 22 // Configurable, see typical pin layout above
#define SS_PIN 21  // Configurable, see typical pin layout above
#define LED_PIN 4  // Define LED pin

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
    pinMode(enA, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(13, OUTPUT);
    pinMode(powerPin, OUTPUT);
    pinMode(LED_PIN, OUTPUT); // Initialize the LED pin as an output

    digitalWrite(powerPin, LOW);
    digitalWrite(LED_PIN, LOW); // Ensure LED is off initially

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
    bool cardPresent = mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial();

    if (cardPresent)
    {
        digitalWrite(LED_PIN, HIGH); // Turn on the LED when a card is detected

        // Capture UID in a single variable
        String uidStr = "";
        for (byte i = 0; i < mfrc522.uid.size; i++)
        {
            uidStr += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "") + String(mfrc522.uid.uidByte[i], HEX);
        }

        // Print the UID
        Serial.print("Card UID: ");
        Serial.println(uidStr);

        if (WiFi.status() == WL_CONNECTED)
        {
            HTTPClient httpPost;
            DynamicJsonDocument doc(1024);
            String requestBody;
            int httpCodePost;

            // Send POST request for UID
            httpPost.begin("https://6686845a378d14d5f751996d--testiotapi.netlify.app/.netlify/functions/api/postUID");
            httpPost.addHeader("Content-Type", "application/json");

            doc.clear(); // Clear previous data
            doc["UID"] = uidStr;
            serializeJson(doc, requestBody);
            Serial.println("Sending UID to server...");
            Serial.println(requestBody); // Print the request body to debug

            httpCodePost = httpPost.POST(requestBody); // Send the POST request
            Serial.print("Status code for UID POST: ");
            Serial.println(httpCodePost); // Print the status code to debug

            if (httpCodePost > 0)
            {
                String payloadPost = httpPost.getString();
                Serial.println(payloadPost);
            }
            else
            {
                Serial.print("POST request for UID failed with status code ");
                Serial.println(httpCodePost);
            }

            httpPost.end(); // End the POST request for UID
        }

        digitalWrite(LED_PIN, LOW); // Turn off the LED after processing the card
    }

    // Tasks to execute regardless of card presence
    digitalWrite(powerPin, HIGH);
    float kelembaban = dht.readHumidity();
    float suhu = dht.readTemperature();

    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient httpPost;
        DynamicJsonDocument doc(1024);
        String requestBody;
        int httpCodePost;

        // Send POST request for temperature and humidity
        httpPost.begin("https://6686845a378d14d5f751996d--testiotapi.netlify.app/.netlify/functions/api/postSuhu");
        httpPost.addHeader("Content-Type", "application/json");

        doc.clear(); // Clear previous data
        doc["suhu"] = suhu;
        doc["kelembapan"] = kelembaban;
        serializeJson(doc, requestBody);

        httpCodePost = httpPost.POST(requestBody); // Send the POST request

        if (httpCodePost > 0)
        {
            String payloadPost = httpPost.getString();
            Serial.println(payloadPost);
        }
        else
        {
            Serial.print("POST request for temperature and humidity failed with status code ");
            Serial.println(httpCodePost);
        }

        httpPost.end(); // End the POST request for temperature and humidity
    }

    // Send GET request for fan speed
    HTTPClient httpGet;
    httpGet.begin("https://6686845a378d14d5f751996d--testiotapi.netlify.app/.netlify/functions/api/getKipas");
    int httpCodeGet = httpGet.GET();

    if (httpCodeGet > 0)
    {
        String payloadGet = httpGet.getString();

        DynamicJsonDocument docGet(1024);
        deserializeJson(docGet, payloadGet);
        kipasSpeed = docGet["kelajuan"]; // Assuming 'kelajuan' is the correct key for fan speed
    }
    else
    {
        Serial.print("GET request failed with status code ");
        Serial.println(httpCodeGet);
    }

    httpGet.end();

    analogWrite(enA, kipasSpeed); // Send PWM signal to fan
    // Set fan to operate in a certain direction
    digitalWrite(in2, LOW);
    digitalWrite(in1, HIGH);
    delay(3000); // Delay for a moment before the next loop iteration
}