# Arduino-HIDTextBot

Arduino-HIDTextBot is a USB HID “text + mouse” bot for boards that support **Keyboard** and **Mouse** emulation (e.g., Arduino Leonardo / Micro / Pro Micro).

When running, it can:
- Open **Windows Notepad** automatically (Win+R → `notepad`).
- Type random pre-defined phrases with **human-like timing**, **typos**, and **backspaces**.
- Occasionally **Alt+Tab** between windows or add **small mouse jitter**.
- Move the mouse cursor along a **shape path** (default: **pentagram**) via `ShapeStepper`.

> ⚠️ **Safety / Responsibility**
> This project can control a computer’s keyboard and mouse. Use only on machines you own or explicitly have permission to test. Be careful: it can type into any focused window.

---

## Features

### HIDTextBot (Keyboard + Mouse)
- Random phrase typing from a phrase list stored in **PROGMEM**.
- Random delays to simulate natural typing.
- Controlled “mistakes”: inserts a wrong char, backspaces, then continues.
- Occasionally:
  - **Alt+Tab** sequences
  - **Small mouse jitter**
- Button handling:
  - **Short press** toggles RUN/STOP (LED indicates running).
  - **Long press (~1.2s)** clears Notepad content (Ctrl+A, Backspace).

### ShapeStepper (Mouse drawing)
- Step-by-step cursor movement along geometric paths:
  - Square
  - Triangle
  - Circle
  - Pentagram (5-point star)
- Can run one explicit shape only (current sketch uses `SH_PENTAGRAM`).
- Adds pauses between completed shapes.

---

## Hardware Requirements

- Arduino board with native USB HID support:
  - **Arduino Leonardo**, **Arduino Micro**, **Pro Micro (ATmega32u4)**, etc.
- **LED** on `D5` (or any pin you configure)
- **Button** on `D7` to GND (uses `INPUT_PULLUP`)

Default wiring:
- LED anode → D5 (with resistor), LED cathode → GND
- Button → D7 and GND (no external resistor needed)

---

## Software Requirements

- Arduino IDE
- Board package for your HID-capable Arduino
- Uses standard Arduino libraries:
  - `Keyboard.h`
  - `Mouse.h`

---

## How It Works (Overview)

### Sketch (`HIDTextBot.ino`)
```cpp
HIDTextBot bot(5, 7);
ShapeStepper mouseShape;
int mouseSizePx = 100;

void setup() {
  randomSeed(analogRead(A0));
  bot.beginNotePad();
  mouseShape.begin(mouseSizePx, false);
  mouseShape.setExplicitShape(SH_PENTAGRAM);
}

void loop() {
  bot.doAction();
  mouseShape.move();
}
```
- beginNotePad() starts HID, waits randomly, opens Notepad, and optionally switches keyboard layout using Win+Space.
- doAction() periodically types a line, Alt+Tabs, or jitters the mouse.
- mouseShape.move() moves the cursor one “step” at a time along the configured shape.

## Controls
## Button (D7)
- Short press: Toggle start/stop (LED reflects running state)
- Long press (~1200ms): Clear content (Ctrl+A, Backspace)
  
> ⚠️  **Note:**
> The provided `HIDTextBot.ino` uses `doAction()` (no button logic).
> 
> If you want button-based start/stop + long-press clear, use `bot.tick()` in `loop()` instead of `bot.doAction()`.

Example:

```cpp
void loop() {
  bot.tick();        // enables button handling + scheduling
  mouseShape.move(); // optional
}
```

---

## Configuration
### Change pins
```cpp
HIDTextBot bot(LED_PIN, BUTTON_PIN);
```
### Adjust delays
```cpp
bot.setDelays(
  24, 187,      // keyMinMs, keyMaxMs
  3300, 8400,   // stepMinMs, stepMaxMs (between actions)
  7000, 15000   // mouseMinMs, mouseMaxMs (jitter interval)
);
```
### Mouse jitter intensity
```cpp
bot.setMouseJitterMax(35);
```
### Change the drawn shape
```cpp
mouseShape.setExplicitShape(SH_SQUARE);
// or SH_TRIANGLE, SH_CIRCLE, SH_PENTAGRAM
```
### Change shape size
```cpp
int mouseSizePx = 100;
mouseShape.begin(mouseSizePx);
```

---

## Notes / Limitations

- Windows Notepad automation uses Win+R and notepad. This is Windows-specific.
- Ensure your Arduino is recognized as a HID keyboard/mouse before uploading.
- The phrase list is stored in flash (PROGMEM) to save RAM.
- If the board is plugged into a machine with sensitive input focus, it may type into the wrong window—use carefully.

---

## Troubleshooting

- Nothing happens:
 - Confirm you’re using an HID-capable board (Leonardo/Micro/Pro Micro).
 - Make sure the correct board/port is selected in Arduino IDE.
- Notepad doesn’t open:
 - Focus might not be on the desktop/session you expect.
 - UAC prompts / locked screen can prevent automation.
- Keyboard layout issues:
 - Disable switchLayout in beginNotePad(...) or adapt the layout switching logic.

---

## License
- [MIT](https://github.com/Obrelix/Arduino-HIDTextBot/blob/main/LICENSE)

---

## Disclaimer
This project is for educational and testing purposes. You are responsible for how you use it.
