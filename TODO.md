# Web Quotes TODO

- [x] E-ink busy pin versus timeout (would have to solder wire to pad).
- [x] Need to actually parse HTTP message (have side C++ project).
- [ ] Change response to allow for different attributions. Something like `[person]quote`
- [ ] Probably do these in side projects to get working code:
  - [x] Logic break text at word bounds?
  - [ ] Drawing a bender Bitmap or small bender icon?
- [ ] Figure out SSID and password stuff. There's an open source lib for this.
- [x] Need to power down after displaying.
- [ ] Timeout/error handling (including non 200 responses).
- [x] "Debug" print macro? Current ifdef stuff is hard to read. Make it a noop if NDEBUG defined.

- [ ] Test project to download response and save to SD card.
- [ ] Put WiFi configuration on file in SD card.

## Version 2.0
- [ ] Change to download a file from github repo once/day or once/week.
  - [ ] Save to SD card
  - [ ] On wake up, read from SD and determine if to re-download or display a different one.
- [ ] Have jumper to force refresh from web.

## Low Priority
- [ ] Better HTTP message building without strcat.
- [ ] Make GitHub repo for StringView?
