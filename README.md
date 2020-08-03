# Web Quotes ESP8266

![Picture of Project](https://raw.githubusercontent.com/Scottz0r/Arduino-WebQuotes/master/images/scottz0r-web-quotes-00.jpg)

This repository contains an Arduino project for an embedded system that downloads quotes from the internet and displays the quotes on an E-Ink screen. This is powered by an ESP8266 WiFi microchip.

This project aims to use as little power as possible, using the deep sleep state to practically eliminate battery use when not refreshing the E-Ink display. This is possible with the super low power state provided by the ESP8266, and the fact that the E-Ink screen does not require power to maintain the display.

State and data are saved to an SD card, which allows data to be cached for long periods of time without the need for connecting to WiFi. WiFi configuration, and logging, is also accomplished by using the SD card capabilities.

## Specs

- ESP8266 Processor
  - 80 MHz Clock
  - 4 MB Flash, ~1 MB flash for program
  - 80 KB SRAM 
- 250x122 E-Ink display
- SD card reader
- 400 mAh Lipoly Battery

## Features

- WiFi configuration and state stored on an SD card.
- Display refreshed every 60 minutes.
- Data downloaded after a configured number of intervals. Cached data used to minimized time powered on.
- Battery Powered.
- Remembers recently shown quotes to reduce repeats.

## Components

- [Adafruit HUZZAH with ESP8266](https://www.adafruit.com/product/2821)
- [Adafruit Monochrome E-Ink Display](https://www.adafruit.com/product/4195)
- [400 mAh Lithium Ion Polymer Battery](https://www.adafruit.com/product/3898)

## Data File Spec

A binary file is used to efficiently fetch quotes when displaying. The overhead of this file format is minimal. All integers in the file are unsigned and stored as little endian. A Python file in the `quote_file_maker` directory creates this file.

The theoretical limit of this file is 65,535 total bytes.

This binary file is kept as a part of this repository as a "free" way to host the file for download on embedded systems.

The file is organized in the following manner:

|Section    |Bytes          |
|-----------|---------------|
|Header     |2              |
|Index      |2 * items      |
|Quotes     |2 + n per item |


### Header

The header is a two byte integer that contains the total number of quotes.

### Index

The index contains an entry for each item. Each index entry is a two byte integer that specifies the starting address of a quote.

To get the starting address of the index entry for a quote index `i`, use: `2 + (i * 2)`

### Quotes

Each quote entry starts with an 8-bit integer that contains the name length and an 8-bit integer that contains the quote length. The name and quote text are ASCII strings that are **not** null terminated.

|Byte   |Description    |
|-------|---------------|
|0      |Name Length    |
|1      |Quote Length   |
|2-*    |Name String    |
|*-\*   |Quote String   |
