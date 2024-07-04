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
String cardUID;

AsyncWebServer server(80);
char data[50]; // Use char array instead of String

void setup()
{
    Serial.begin(9600); // Initialize serial communications with the PC
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

    server.begin();
    // while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();                       // Init SPI bus
    mfrc522.PCD_Init();                // Init MFRC522
    delay(4);                          // Optional delay. Some board do need more time after init to be ready, see Readme
    mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
    Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop()
{
    // Reset the cardUID
    cardUID = "";
    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if (!mfrc522.PICC_IsNewCardPresent())
    {
        return;
    }

    // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial())
    {
        return;
    }

    // Print the UID
    Serial.print("Card UID:");
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        // Store the UID in the cardUID variable
        if (mfrc522.uid.uidByte[i] < 0x10)
        {
            cardUID += "0";
        }
        else
        {
            cardUID += " ";
        }
        cardUID += String(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println(cardUID);

    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient httpPost;

        // Send POST request
        httpPost.begin("http://192.168.1.3:3000/postRfid");     // Specify the URL
        httpPost.addHeader("Content-Type", "application/json"); // Specify the content type

        // Create the JSON object
        DynamicJsonDocument doc(1024);
        doc["id"] = String(cardUID).trim();
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

        delay(1000);
    }
}