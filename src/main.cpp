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

int coolTemp = 180;
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
    rest.variable("coolTemp", &coolTemp);
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
    //+rest.function("led", ledControl);

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
        if (CAN.packetId() == 0x0CFFF048) //TPS CAN data send
        {
            byte lowByte = Bytearray[2];
            byte highByte = Bytearray[3];
            tps = ((highByte * 256) + lowByte)/10;
        }
        if (CAN.packetId() == 0x0CFFF548) //Voltage/Coolant CAN data send
        {
            byte lowByte = Bytearray[0];
            byte highByte = Bytearray[1];
            voltage = ((highByte * 256) + lowByte)/100;
            lowByte = Bytearray[4];
            highByte = Bytearray[5];
            coolTemp = ((highByte * 256) + lowByte) / 10;
        }
        if (CAN.packetId() == 0x0CFFF148) //afr CAN data send
        {
            byte lowByte = Bytearray[4];
            byte highByte = Bytearray[5];
            afr = ((highByte * 256) + lowByte) / 100;
        }
        if (CAN.packetId() == 0x0CFFF008) //EGT1,2,3,4 CAN data send
        {
            byte lowByte = Bytearray[0];
            byte highByte = Bytearray[1];
            EGT1 = ((highByte * 256) + lowByte);
            lowByte = Bytearray[2];
            highByte = Bytearray[3];
            EGT2 = ((highByte * 256) + lowByte);
            lowByte = Bytearray[4];
            highByte = Bytearray[5];
            EGT3 = ((highByte * 256) + lowByte);
            lowByte = Bytearray[6];
            highByte = Bytearray[7];
            EGT4 = ((highByte * 256) + lowByte);
        }
    }
}