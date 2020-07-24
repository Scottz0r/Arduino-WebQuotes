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
constexpr auto SRAM_CS = -1; // Cannot use because on same pin as reset.
constexpr auto EPD_CS = 0;
constexpr auto EPD_DC = 15;
constexpr auto EPD_RESET = -1;
constexpr auto EPD_BUSY = -1; // No connected on featherwing.

// Would also be great to config?
const char* host = "192.168.1.101";
const int http_port = 5000;

// Make this a "safe buffer"
constexpr auto BUFFER_SIZE = 1024 * 4;
char buffer[BUFFER_SIZE];

Adafruit_SSD1675 display(250, 122, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

void setup()
{
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);

    Serial.begin(115200);

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

    auto request_size = fetch_web();
    if(request_size == 0)
    {
        return;
    }

    // Parse out HTTP body and send to display.
    auto buffer_slice = StringSlice(buffer, request_size);
    auto http_body = http::get_body(buffer_slice);

    wrap_and_display(http_body);

    // Go to deep sleep. Requires pin 16 to be wired to reset. 60e6 = 1 minute.
    Serial.println("Going to sleep...");
    constexpr auto sleep_time = (uint64_t)60e6 * 5;
    ESP.deepSleep(sleep_time);
}

std::size_t fetch_web()
{
    Serial.print("Connecting to ");
    Serial.println(host);

    WiFiClient client;
    if(!client.connect(host, http_port))
    {
        Serial.println("Failed to connect to host.");
        return 0;
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

    return i;
}

// void write_to_display(const StringSlice& text)
// {
//     display.clearBuffer();
//     // Set down a few pixels because the font is much higher than the default 6px baseline.
//     display.setCursor(0, 10);
//     display.setTextSize(1);
//     // TODO: Wrap at a word boundary instead of any character.
//     display.setFont(&FreeSerif9pt7b);
//     display.setTextColor(EPD_BLACK);
//     display.setTextWrap(true);

//     display.write(text.data(), text.size());

//     display.display();

//     Serial.print("Display done!\r\n");
// }

static StringSlice get_word(StringSlice& buffer)
{
    // TODO: Find ,.;:\n etc.
    auto pos = buffer.find(' ');

    if (pos != StringSlice::npos)
    {
        // If word found, return the word and jump the input buffer after the word and whitespace/punctuation.
        auto word = buffer.substr(0, pos);
        buffer = buffer.substr(pos + 1);
        return word;
    }
    else
    {
        // When on the last word, set the buffer to empty and return the contents of the buffer.
        auto word = buffer;
        buffer = StringSlice();
        return word;
    }
}

static void wrap_and_display(const StringSlice& text)
{
    StringSlice ss = text;

    constexpr auto line_height = 16; // pixels
    
    constexpr auto word_buffer_size = 64;
    char word_buffer[word_buffer_size];

    display.clearBuffer();
    display.setTextColor(EPD_BLACK);
    display.setFont(&FreeSerif9pt7b);
    display.setTextSize(1);
    display.setTextWrap(false);
    display.setCursor(0, line_height);

    // TODO: Leftmost bound?

    while(!ss.empty())
    {
        // TODO: Better buffer copying?
        auto word = get_word(ss);
        if(word.size() < word_buffer_size)
        {
            memcpy(word_buffer, word.data(), word.size());
            word_buffer[word.size()] = 0;
        }
    
        int16_t x1, y1;
        uint16_t width, height;

        display.getTextBounds(word_buffer, display.getCursorX(), display.getCursorY(), &x1, &y1, &width, &height);

        if(x1 + width > display.width())
        {
            // Wrap using GFX's library newline handling.
            display.write('\r');
            display.write('\n');
        }

        display.print(word_buffer);

        // Write a space after each word.
        display.write(' ');
    }

    display.display();
}

void loop()
{
    // Do nothing.
}