#include "web_downloader.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SD.h>
#include <StringSlice.h>

#include "quote_manager.h"
#include "config_manager.h"
#include "debug_serial.h"
#include "sys_config.h"

namespace scottz0r
{
namespace web
{
    static void disconnect_wifi();
    static bool download_file(const WebConfig& config);
    static bool init_wifi(const WebConfig& config);
    static bool is_status_200(StringSlice status_line);
    static bool swap_files();
    
    bool download_file()
    {
        // Make sure configuration has been loaded. Assumes config was loaded before this method call.
        if(!config)
        {
            return false;
        }

        // Initialize WiFi connection
        if(!init_wifi(config.get()))
        {
            disconnect_wifi();
            return false;
        }

        // Download file
        if(!download_file(config.get()))
        {
            disconnect_wifi();
            return false;
        }

        disconnect_wifi();

        // Swap files.
        if(!swap_files())
        {
            return false;
        }

        return true;
    }

    static bool init_wifi(const WebConfig& config)
    {
        DEBUG_PRINT("Connecting to ");
        DEBUG_PRINTLN(config.wifi_ssid);

        WiFi.begin(config.wifi_ssid, config.wifi_pwd);

        while(WiFi.status() != WL_CONNECTED)
        {
            delay(250);
            DEBUG_PRINT(".");
        }

        // TODO: A timeout?

        return true;
    }

    bool is_status_200(StringSlice status_line)
    {
        auto pos = status_line.find(' ');
        if (pos != StringSlice::npos)
        {
            status_line = status_line.substr(pos + 1);
            auto code = status_line.substr(0, 3);

            if (code == "200")
            {
                return true;
            }
        }

        return false;
    }

    static void disconnect_wifi()
    {
        if(WiFi.status() == WL_CONNECTED)
        {
            WiFi.disconnect();
        }
    }

    static bool download_file(const WebConfig& config)
    {
        DEBUG_PRINTLN("Connecting to host...");

        WiFiClientSecure client;
        client.setInsecure(); // Don't validate certificates. Could have MiM attack, but not much to "attack".
        
        if(!client.connect(config.cfg_host, config.cfg_port))
        {
            DEBUG_PRINTLN("Failed to connect to host.");
            return false;
        }

        char buffer[300];
        strcpy(buffer, "GET ");
        strcat(buffer, config.cfg_path);
        strcat(buffer, " HTTP/1.1\r\n");
        strcat(buffer, "Host: ");
        strcat(buffer, config.cfg_host);
        strcat(buffer, "\r\nConnection: close\r\n\r\n");

        DEBUG_PRINTLN("Sending this request: ");
        DEBUG_PRINTLN(buffer);

        if(!client.print(buffer))
        {
            DEBUG_PRINTLN("Failed to send request.");
            client.stop();
            return false;
        }

        // Get status line of request and make sure 200.
        std::size_t bytes_read = client.readBytesUntil('\n', buffer, sizeof(buffer));

        StringSlice status_line(buffer, bytes_read);
        if (!is_status_200(status_line))
        {
            DEBUG_PRINTLN("Status not 200!");
            client.stop();
            return false;
        }

        // Skip over other HTTP headers. Skip until an empty line.
        bytes_read = client.readBytesUntil('\n', buffer, sizeof(buffer));
        while (bytes_read > 1 || (bytes_read == 1 && buffer[0] != '\r'))
        {
            bytes_read = client.readBytesUntil('\n', buffer, sizeof(buffer));
        }

        // TODO: Move SD stuff somewhere else?
        DEBUG_PRINTLN("Removing existing file...");
        if(SD.exists(download_filename))
        {
            SD.remove(download_filename);
        }

        DEBUG_PRINTLN("Creating file...");
        auto file = SD.open(download_filename, FILE_WRITE);
        if(!file)
        {
            DEBUG_PRINTLN("Failed to open file.");
            client.stop();
            return false;
        }

        DEBUG_PRINTLN("Writing body to SD card...");
        while(client.connected() || client.available())
        {
            if(client.available())
            {
                char c = client.read();
                file.write(c);
            }
        }

        client.stop();
        file.close();
        DEBUG_PRINTLN("Download complete!");
        return true;
    }

    static bool swap_files()
    {
        DEBUG_PRINT("Moving/swapping \"");
        DEBUG_PRINT(download_filename);
        DEBUG_PRINT("\" with \"");
        DEBUG_PRINT(data_filename);
        DEBUG_PRINTLN("\"...");

        if(SD.exists(data_filename))
        {
            SD.remove(data_filename);
        }

        auto dl_file = SD.open(download_filename, FILE_READ);
        if(!dl_file)
        {
            DEBUG_PRINTLN("Failed to open downloaded file.");
            return false;
        }

        auto data_file = SD.open(data_filename, FILE_WRITE);
        if(!data_file)
        {
            DEBUG_PRINTLN("Failed to open data file.");
            dl_file.close();
            return false;
        }

        while(dl_file.available())
        {
            data_file.write(dl_file.read());
        }

        data_file.close();
        dl_file.close();

        // Cleanup "temporary" download file after copy.
        SD.remove(download_filename);

        return true;
    }
}}
