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

