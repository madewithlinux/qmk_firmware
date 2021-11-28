# notes.md

# general build environment setup
```bash
sudo apt-get install gcc unzip wget zip gcc-avr binutils-avr avr-libc dfu-programmer dfu-util gcc-arm-none-eabi binutils-arm-none-eabi libnewlib-arm-none-eabi
```

# KBD75

## layout
`keyboards/kbdfans/kbd75/keymaps/madewithlinux/keymap.c`

## udev setup
(ref `docs/faq_build.md`)
```bash
sudo tee -a /etc/udev/rules.d/50-atmel-dfu.rules << EOF
# Atmel ATMega32U4
SUBSYSTEMS=="usb", ATTRS{idVendor}=="03eb", ATTRS{idProduct}=="2ff4", MODE:="0666"
# Atmel USBKEY AT90USB1287
SUBSYSTEMS=="usb", ATTRS{idVendor}=="03eb", ATTRS{idProduct}=="2ffb", MODE:="0666"
# Atmel ATMega32U2
SUBSYSTEMS=="usb", ATTRS{idVendor}=="03eb", ATTRS{idProduct}=="2ff0", MODE:="0666"
EOF
sudo udevadm control --reload-rules
sudo udevadm trigger
```

## build
```bash
make kbdfans/kbd75:madewithlinux:dfu
```

## tabalt special sauce
see also: http://joools.xyz/wiki/winxp-style-alt-tab

TODO: maybe some options in `docs/config_options.md` can help?

misc links about graphviz
* http://magjac.com/graphviz-visual-editor/
* vscode extensions
  * https://marketplace.visualstudio.com/items?itemName=tintinweb.graphviz-interactive-preview
  * https://marketplace.visualstudio.com/items?itemName=joaompinto.vscode-graphviz
  * https://marketplace.visualstudio.com/items?itemName=thankcreate.power-fsm-viewer
  * https://github.com/thankcreate/power-fsm-viewer/wiki/Attributes
* documentation
  * https://graphviz.gitlab.io/doc/info/shapes.html
  * https://graphviz.gitlab.io/doc/info/attrs.html
* how to
  * https://graphviz.org/Gallery/directed/fsm.html
  * https://martin-thoma.com/how-to-draw-a-finite-state-machine/
  * https://projectacrn.github.io/latest/developer-guides/graphviz.html
