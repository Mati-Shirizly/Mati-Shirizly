# ppg-motion: ESP32 + MAX30105 raw PPG streamer

Firmware that streams raw **red / IR / green** samples from one or more MAX30105
modules to a PC over USB serial. It is the acquisition side of using PPG light
channels to measure mechanical movement:
- with one module on skin, separate motion from pulse using the three wavelengths
- with several modules, trilaterate a moving surface

## Hardware

| MAX30105 breakout | ESP32 (DevKit) |
|---|---|
| VIN | 3V3 |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |
| INT | not used (FIFO is polled) |

Power the breakout from **3.3 V** so its I2C pull-ups don't pull the ESP32 pins to 5 V.

**More than one sensor:** every MAX30105 has the fixed address `0x57`. You have two options:
- **Two sensors:** put the second one on the ESP32's second I2C bus (GPIO 25 = SDA, 26 = SCL by default).
- **Up to 8 per bus:** use a **TCA9548A** multiplexer (address `0x70`). Wire the ESP32 to the mux's SDA/SCL and each sensor to one mux channel (SDx/SCx).

Then list the sensors in `firmware/src/config.h`:

```cpp
constexpr SensorSlot SENSOR_SLOTS[] = {
    {0, 0}, {0, 1}, {0, 2},   // bus 0, TCA9548A channels 0..2
};
```

## Build and flash

PlatformIO (VS Code extension or CLI):

```bash
cd ppg-motion/firmware
pio run -t upload
pio device monitor        # 921600 baud
```

The firmware has no external library dependencies. `src/max30105.*` is a small
register-level driver. It drains the whole 32-sample hardware FIFO each read and
reports overflows. The SparkFun library's `check()` silently drops samples when
more than 3 are waiting.

For the Arduino IDE, create a sketch folder `ppg_motion/`, copy `main.cpp` in as
`ppg_motion.ino`, and copy `config.h`, `max30105.h` and `max30105.cpp` next to it.

## Output format

```
# ppg-motion firmware | sensors=1 led_mode=3 sr=200 avg=1 pw=411us range=16384nA out_rate=200.0Hz streaming=1
# columns: sensor,seq,t_us,red,ir,green
0,0,202000,81234,95012,40211
0,1,206000,81240,95020,40209
```

- `sensor`: index in `SENSOR_SLOTS`
- `seq`: per-sensor sample counter. **This is the time base: `t = seq / out_rate`.**
  If samples are lost, `seq` jumps and a `# overflow sensor=.. lost=..` line is printed.
- `t_us`: ESP32 `micros()` estimate, for aligning sensors with each other or with an IMU
- `red`, `ir`, `green`: raw 18-bit ADC counts (0–262143). Unused LEDs read 0.

Lines that start with `#` are status messages. Skip them when parsing.

## Serial commands

| Command | Effect |
|---|---|
| `start` / `stop` | start or stop streaming |
| `info` | configuration, sensor status, lost-sample counts |
| `led <s\|*> <red> <ir> <green>` | LED current 0–255 (≈0–50 mA) for sensor `s` or all |
| `temp` | die temperature per sensor (useful for drift compensation) |
| `reset` | re-initialise all sensors |

## Tuning for motion measurement

- **LED currents:** set them so each channel's DC level sits at about **50–80 % of full scale**
  (130k–210k counts). Saturation clips exactly the motion transients you want to measure.
- **Default rate:** 200 Hz with a 411 µs pulse width (18 bit) and 3 LEDs. This covers
  tremor (4–12 Hz), gait and vibration up to about 90 Hz.
- **Faster sampling:** lower `PULSE_WIDTH_US` to 215/118 and raise `SAMPLE_RATE` to 400/800.
  With 3 LEDs, the datasheet's sample-rate × pulse-width table sets the upper limit.
- **Serial bandwidth:** one line is about 40 bytes. At 921600 baud the link carries
  roughly 2000 lines/s across all sensors.
- **Crosstalk:** several modules aimed at the same surface will see each other's LEDs.
  Shield them, or run a test with `led <s> 0 0 0` on the others to measure the crosstalk.
- **Ground truth:** add an IMU on the same mount when you validate the motion signal.
