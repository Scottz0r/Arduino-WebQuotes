# Web Quotes TODO

- [x] E-ink busy pin versus timeout (would have to solder wire to pad).
- [x] Need to actually parse HTTP message (have side C++ project).
- [x] Change response to allow for different attributions. Something like `[person]quote`
- [ ] Probably do these in side projects to get working code:
  - [x] Logic break text at word bounds?
  - [ ] Drawing a bender Bitmap or small bender icon?
- [x] Need to power down after displaying.
- [ ] Timeout/error handling (including non 200 responses).
- [x] "Debug" print macro? Current ifdef stuff is hard to read. Make it a noop if NDEBUG defined.
- [x] Test project to download response and save to SD card.
- [x] Put WiFi configuration on file in SD card.
- [ ] Verify downloaded data file for assumptions (max length on name and text).
- [ ] Better file reading and test of API/assumptions.
  - [ ] Counting quotes and EOF handling. Does current code work as expected without an newline at EOF?
- [ ] Make it select a random number, based off of total number of quotes, and show that.
  - [ ] After n-times doing this, redownload a list form the internet.
- [ ] Configure refresh interval (CFG file).
- [ ] Debug log to SD card (LOG.TXT).
  - [ ] Somehow enable/disable serial logging?
- [ ] Make char buffer stuff more safe
  - [ ] Make a method to copy StringSlice to null terminated string char* buffer.

## Version 2.0
- [ ] Change to download a file from github repo every n-times.
  - [x] Save to SD card
  - [x] On wake up, read from SD and determine if to re-download or display a different one.
- [ ] Have jumper to force refresh from web.

## Low Priority
- [ ] Better HTTP message building without strcat.
- [ ] Make GitHub repo for StringView?
