# itg_wheel

![itg_wheel](imgur.com image replace me!)

*A short description of the keyboard/project*

* Keyboard Maintainer: [madewithlinux](https://github.com/madewithlinux)
* Hardware Supported: *The PCBs, controllers supported*
* Hardware Availability: *Links to where you can find this hardware*

Make example for this keyboard (after setting up your build environment):

    make itg_wheel:default

Flashing example for this keyboard:

    make itg_wheel:default:flash

```bash
qmk flash -kb itg_wheel -km default
```

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 3 ways:

* **Bootmagic reset**: Hold down the key at (0,0) in the matrix (usually the top left key or Escape) and plug in the keyboard
* **Physical reset button**: Briefly press the button on the back of the PCB - some may have pads you must short instead
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available


# stepmania/ITG default keyboard mappings

ref `itgmania/src/InputMapper.cpp:57`

| keyboard key  | game button   |       |
|---------------|---------------|-------|
| KEY_LEFT      | P1 MENU LEFT  | false |
| KEY_RIGHT     | P1 MENU RIGHT | false |
| KEY_UP        | P1 MENU UP    | false |
| KEY_DOWN      | P1 MENU DOWN  | false |
| KEY_ENTER     | P1 START      | false |
| KEY_SLASH     | P1 SELECT     | false |
| KEY_ESC       | P1 BACK       | false |
| KEY_KP_C4     | P2 MENU LEFT  | true  |
| KEY_KP_C6     | P2 MENU RIGHT | true  |
| KEY_KP_C8     | P2 MENU UP    | true  |
| KEY_KP_C2     | P2 MENU DOWN  | true  |
| KEY_KP_ENTER  | P2 START      | true  |
| KEY_KP_C0     | P2 SELECT     | true  |
| KEY_BACKSLASH | P2 BACK       | true  |
| KEY_F1        | COIN          | false |
| KEY_SCRLLOCK  | OPERATOR      | false |

ref `itgmania/src/GameManager.cpp:112`

| keyboard key    | game button   |       |
|-----------------|---------------|-------|
| KEY_DEL         | P1 MENU LEFT  | false |
| KEY_PGDN        | P1 MENU RIGHT | false |
| KEY_HOME        | P1 MENU UP    | false |
| KEY_END         | P1 MENU DOWN  | false |
| KEY_LEFT        | P1 LEFT       | false |
| KEY_RIGHT       | P1 RIGHT      | false |
| KEY_UP          | P1 UP         | false |
| KEY_DOWN        | P1 DOWN       | false |
| KEY_KP_SLASH    | P2 MENU LEFT  | true  |
| KEY_KP_ASTERISK | P2 MENU RIGHT | true  |
| KEY_KP_HYPHEN   | P2 MENU UP    | true  |
| KEY_KP_PLUS     | P2 MENU DOWN  | true  |
| KEY_KP_C4       | P2 LEFT       | true  |
| KEY_KP_C6       | P2 RIGHT      | true  |
| KEY_KP_C8       | P2 UP         | true  |
| KEY_KP_C2       | P2 DOWN       | true  |
| KEY_KP_C7       | P2 UPLEFT     | true  |
| KEY_KP_C9       | P2 UPRIGHT    | true  |


