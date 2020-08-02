# Web Quotes TODO

- [x] E-ink busy pin versus timeout (would have to solder wire to pad).
- [x] Need to actually parse HTTP message (have side C++ project).
- [x] Change response to allow for different attributions. Something like `[person]quote`
- [x] Probably do these in side projects to get working code:
  - [x] Logic break text at word bounds?
- [x] Need to power down after displaying.
- [x] Timeout/error handling (including non 200 responses).
- [x] "Debug" print macro? Current ifdef stuff is hard to read. Make it a noop if NDEBUG defined.
- [x] Test project to download response and save to SD card.
- [x] Put WiFi configuration on file in SD card.
- [x] Verify downloaded data file for assumptions (max length on name and text).
- [x] Counting quotes and EOF handling. Does current code work as expected without an newline at EOF?
- [x] Verify file is valid before replacing existing file.
  - [x] Maximum length
  - [x] Max name size (63)
  - [x] Max text size (???)
- [x] Make it select a random number, based off of total number of quotes, and show that.
  - [x] After n-times doing this, redownload a list form the internet.
- [x] Display module.
- [x] Configure refresh interval (CFG file).
- [x] Debug log to SD card (LOG.TXT).
- [x] Make char buffer stuff more safe
  - [x] Review for unsafe code.
  - [x] Make a method to copy StringSlice to null terminated string char* buffer.
  - [x] Maybe a better "get_line" method for streams? That always read to a newline (or EoF).
- [x] Try to clean up `setup()` method and make flow a bit better? Maybe make a hard-error method when multiple things can cause exit conditions?
- [x] Have data file specify count at top of file (to save processing).
- [x] Better random number: Don't pick a quote that has shown up in the last n times.
- [x] Better state: Save as a binary file for performance.
- [x] Test project: Can SecureClient make request to HTTP? Do I need logic to load another WiFiClient depending on HTTPS?
  - Answer: No. But, this is low priority because of free file host on GitHub.
- [x] Make data file binary for better performance.
  - [x] Python to generate file?
  - [x] Note: Processor is little endian.

## Version 2.0
- [x] Change to download a file from github repo every n-times.
  - [x] Save to SD card
  - [x] On wake up, read from SD and determine if to re-download or display a different one.
- [x] Have jumper to force refresh from web (don't do. Can do this from modifying state file).

## Low Priority
- [x] Make GitHub repo for StringSlice?
  - [x] Use in this project.
