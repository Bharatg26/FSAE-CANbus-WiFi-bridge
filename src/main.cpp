#include <Arduino.h>
#include <CAN.h>
#include <WiFi.h>
#include <aREST.h>

// Create aREST instance
aREST rest = aREST();

// WiFi parameters
const char *ssid = "BR18";
const char *password = "password";

// The port to listen for incoming TCP connections
#define LISTEN_PORT 80

// Create an instance of the server
WiFiServer server(LISTEN_PORT);

int waterTemp = 180;
int oilTemp = 240;
int oilPres = 200;
int tps = 77;
int afr = 12;

int EGT1 = 431;
int EGT2 = 432;
int EGT3 = 400;
int EGT4 = 450;

double voltage = 12.7;

// Declare functions to be exposed to the API
int ledControl(String command);
void setup(void)
{
    Serial.begin(9600);
    rest.variable("coolTemp", &waterTemp);
    rest.variable("oilTemp", &oilTemp);
    rest.variable("oilPres", &oilPres);
    rest.variable("vBat", &voltage);
    rest.variable("TPS", &tps);
    rest.variable("AFR", &afr);

    rest.variable("EGT1", &EGT1);
    rest.variable("EGT2", &EGT2);
    rest.variable("EGT3", &EGT3);
    rest.variable("EGT4", &EGT4);

    // Function to be exposed
    //rest.function("led", ledControl);

    // Give name & ID to the device (ID should be 6 characters long)
    rest.set_id("000001");
    rest.set_name("BR18");

    // Setup WiFi network
    WiFi.softAP(ssid, password);
    Serial.println("");
    Serial.println("WiFi created");

    // Start the server
    server.begin();
    Serial.println("Server started");

    delay(100);

    IPAddress Ip(192, 168, 1, 1);
    IPAddress NMask(255, 255, 255, 0);
    WiFi.softAPConfig(Ip, Ip, NMask);

    // Print the IP address
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);

    CAN.setPins(4, 5);
    CAN.begin(250E3);

    delay(500);
    if (!CAN.begin(250E3))
        Serial.println("Starting CAN failed!");
    else
        Serial.println("Starting CAN success!");
}

void loop()
{
    int packetsent = CAN.parsePacket();
    if (packetsent)
    {
        Serial.print("Frame ID: ");
        Serial.println(CAN.packetId());
        int Bytearray[CAN.available()];
        for (int i = 0; i < CAN.packetDlc(); i++)
        {
            Bytearray[i] = CAN.read();
            Serial.print("Byte ");
            Serial.print(i);
            Serial.print(": ");
            Serial.println(Bytearray[i]);
        }
    }
}