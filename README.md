# firmware

Source assets for embedded computers.

## Targets

### Native Builds

* `mk n` - build, then run a `native` application
* `mk nc` - build, then run a `native` application, then host code coverage

## System Dependencies

See
[the wiki](https://github.com/project-81/firmware/wiki#getting-started)
for notes on getting set up.

### Required

* [crosstool-ng](https://crosstool-ng.github.io/) (`ct-ng`)
* [python](https://github.com/python/cpython) (3.11 or newer recommended)
* [vmklib](https://github.com/vkottler/vmklib) (`mk`, install via `pip`)
* [yambs](https://github.com/vkottler/yambs) (`mbs`, install via `pip`)

### Optional

* [ctags](https://github.com/universal-ctags/ctags)
* [picotool](https://github.com/raspberrypi/picotool)

## Resources

* [J-Link GDB Server](https://wiki.segger.com/J-Link_GDB_Server)
