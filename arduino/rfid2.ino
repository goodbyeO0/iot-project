#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

#define RST_PIN 22 // Configurable, see typical pin layout above
#define SS_PIN 21  // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

const char *ssid = "afik uitm";
const char *password = "afikjasin";

AsyncWebServer server(80);

void setup()
{
    Serial.begin(9600); // Initialize serial communications with the PC

    // Start the WiFi connection process
    WiFi.begin(ssid, password);

    // Wait for the connection to be established
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.begin();
    while (!Serial)
        ;               // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522
    Serial.println("Approximate your card to the reader...");
    Serial.println();
}

void loop()
{
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

    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient httpPost;

        // Send POST request
        httpPost.begin("http://192.168.178.122:3005/postUID");  // Specify the URL
        httpPost.addHeader("Content-Type", "application/json"); // Specify the content type

        // Create the JSON object
        DynamicJsonDocument doc(1024);
        doc["UID"] = uidStr;
        String requestBody;
        serializeJson(doc, requestBody);
        Serial.println(uidStr);

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
        delay(1000); // Pause before next read
    }
}