#include <Arduino.h>
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

    pinMode(13, OUTPUT);

    Serial.begin(9600);

    // Init variables and expose them to REST API
    // rest.variable("coolTemp (f)", &waterTemp);
    // rest.variable("oilTemp (f)", &oilTemp);
    // rest.variable("oilPres (psi)", &oilPres);
    // rest.variable("vBat", &voltage);
    // rest.variable("TPS (%)", &tps);
    // rest.variable("AFR", &afr);

    // rest.variable("EGT1 (f)", &EGT1);
    // rest.variable("EGT2 (f)", &EGT2);
    // rest.variable("EGT3 (f)", &EGT3);
    // rest.variable("EGT4 (f)", &EGT4);

    // Init variables and expose them to REST API
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
    rest.function("led", ledControl);

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
}

void loop()
{

    // Handle REST calls
    WiFiClient client = server.available();
    // if (!client)
    // {
    //     return;
    // }
    // while (!client.available())
    // {
    //     delay(1);
    // }
    rest.handle(client);

    waterTemp += 1;
    oilTemp += 1;
    voltage -= 0.01;

    delay(500);

    digitalWrite(13, !digitalRead(13));
}

// Custom function accessible by the API
int ledControl(String command)
{

    // Get state from command
    int state = command.toInt();

    digitalWrite(13, state);
    return state;
}