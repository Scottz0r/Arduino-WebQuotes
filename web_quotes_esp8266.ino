#include <Arduino.h>
#include <string.h>

#include <ESP8266WiFi.h>

#include <Adafruit_GFX.h>
#include <Adafruit_EPD.h>
#include <Fonts/FreeSerif9pt7b.h>

#include "string_slice.h"
#include "simple_http_parser.hpp"

// SSID & password
#include "secrets.h"

using namespace scottz0r;

// E-Ink configuration
constexpr auto SD_CS = 2;
constexpr auto SRAM_CS = 16;
constexpr auto EPD_CS = 0;
constexpr auto EPD_DC = 15;
constexpr auto EPD_RESET = -1;
constexpr auto EPD_BUSY = -1; // TODO: Would be nice to figure out how to get this to work.

// Would also be great to config?
const char* host = "192.168.1.101";
const int http_port = 5000;

// Make this a "safe buffer"
constexpr auto BUFFER_SIZE = 1024 * 4;
char buffer[BUFFER_SIZE];

Adafruit_SSD1675 display(250, 122, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

void write_to_display();
void fetch_web();

void setup()
{
    pinMode(0, OUTPUT);
    digitalWrite(0, LOW);

    Serial.begin(115200);
    delay(100);

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(CFG_SSID);

    // Display initialization.
    display.begin();

    // WiFi initialization.
    WiFi.begin(CFG_SSID, CFG_PASSWORD);

    while(WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi Connected");
    Serial.println("IP Address: ");
    Serial.println(WiFi.localIP());

    fetch_web();
}

void fetch_web()
{
    Serial.print("Connecting to ");
    Serial.println(host);

    WiFiClient client;
    if(!client.connect(host, http_port))
    {
        Serial.println("Failed to connect to host.");
        return;
    }

    String url = "/";
    Serial.print("Requesting URL: ");
    Serial.println(url);

    // This will send the request to the server
    // client.print(String("GET ") + url + " HTTP/1.1\r\n" +
    //             "Host: " + host + "\r\n" + 
    //             "Connection: close\r\n\r\n");    
    buffer[0] = 0;
    strcpy(buffer, "GET / HTTP/1.1\r\n");
    strcat(buffer, "Host: ");
    strcat(buffer, host);
    strcat(buffer, "\r\nConnection: close\r\n\r\n");
    client.print(buffer);

    // Read all the lines of the reply from server and print them to Serial
    size_t i = 0;
    while(client.connected() || client.available())
    {
        if(client.available())
        {
            // String line = client.readStringUntil('\r');
            // Serial.print(line);
            buffer[i] = client.read();
            ++i;

            // Overflow condition: Way too big, so stop. Our web service should know of embedded limits.
            if(i == BUFFER_SIZE)
            {
                // TODO: Error and not display.
                --i;
                break;
            }
        }
    }

    // Always null terminate buffer.
    buffer[i] = 0;

    Serial.write(buffer, i);

    Serial.print("\r\n");
    Serial.print("HTTP message used ");
    Serial.print(i);
    Serial.print(" bytes in buffer (");
    Serial.print(BUFFER_SIZE);
    Serial.print(" max).\r\n");

    Serial.print("Now displaying...\r\n\r\n");
    
    client.stop();

    auto buffer_slice = StringSlice(buffer, i);
    auto http_body = http::get_body(buffer_slice);

    write_to_display(http_body);

    // TODO: Send to power down mode.
}

void write_to_display(const StringSlice& text)
{
    display.clearBuffer();
    // Set down a few pixels because the font is much higher than the default 6px baseline.
    display.setCursor(0, 10);
    display.setTextSize(1);
    // TODO: Wrap at a word boundary instead of any character.
    display.setFont(&FreeSerif9pt7b);
    display.setTextColor(EPD_BLACK);
    display.setTextWrap(true);

    display.write(text.data(), text.size());

    display.display();

    Serial.print("Display done!\r\n");
}

void loop()
{
    // Do nothing.
}