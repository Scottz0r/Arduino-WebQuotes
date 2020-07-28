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
- [ ] Display module.
- [ ] Configure refresh interval (CFG file).
- [ ] Debug log to SD card (LOG.TXT).
  - [ ] Somehow enable/disable serial logging?
- [ ] Make char buffer stuff more safe
  - [ ] Make a method to copy StringSlice to null terminated string char* buffer.
  - [x] Maybe a better "get_line" method for streams? That always read to a newline (or EoF).

- [x] Test project: Can SecureClient make request to HTTP? Do I need logic to load another WiFiClient depending on HTTPS?
  - Answer: No. But, this is low priority because of free file host on GitHub.

## Version 2.0
- [x] Change to download a file from github repo every n-times.
  - [x] Save to SD card
  - [x] On wake up, read from SD and determine if to re-download or display a different one.
- [x] Have jumper to force refresh from web (don't do. Can do this from modifying state file).

## Low Priority
- [ ] Better HTTP message building without strcat.
- [ ] Make GitHub repo for StringView?
