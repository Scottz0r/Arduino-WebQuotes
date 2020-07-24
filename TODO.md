# Web Quotes TODO

- [x] E-ink busy pin versus timeout (would have to solder wire to pad).
- [x] Need to actually parse HTTP message (have side C++ project).
- [ ] Probably do these in side projects to get working code:
  - [x] Logic break text at word bounds?
  - [ ] Drawing a bender Bitmap or small bender icon?
- [ ] Different fonts? Roboto and Roboto Slab? Noto Serif?
- [ ] Figure out SSID and password stuff. There's an open source lib for this.
- [x] Need to power down after displaying.
- [ ] Timeout/error handling (including non 200 responses).
- [ ] "Debug" print macro? Current ifdef stuff is hard to read. Make it a noop if NDEBUG defined.

## Low Priority
- [ ] Better HTTP message building without strcat.
- [ ] Make GitHub repo for StringView?
