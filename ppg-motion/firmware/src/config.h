#pragma once
#include <Arduino.h>

// ---------------------------------------------------------------------------
// Serial link
// ---------------------------------------------------------------------------
// One CSV line is ~40 bytes. 4 sensors x 200 Hz = 800 lines/s = ~32 kB/s,
// so keep the baud rate >= 500000 when streaming several sensors.
constexpr uint32_t SERIAL_BAUD = 921600;

// ---------------------------------------------------------------------------
// I2C buses (the ESP32 has two hardware I2C controllers)
// ---------------------------------------------------------------------------
constexpr int I2C0_SDA = 21;
constexpr int I2C0_SCL = 22;
constexpr int I2C1_SDA = 25;  // only used if a sensor is placed on bus 1
constexpr int I2C1_SCL = 26;
constexpr uint32_t I2C_CLOCK_HZ = 400000;

// TCA9548A I2C multiplexer (needed for >2 MAX30105, they all sit at 0x57)
constexpr uint8_t TCA9548A_ADDR = 0x70;

// ---------------------------------------------------------------------------
// Sensor map. One entry per MAX30105.
//   bus        : 0 -> Wire (I2C0 pins), 1 -> Wire1 (I2C1 pins)
//   muxChannel : 0..7 = TCA9548A channel on that bus, -1 = connected directly
// Examples:
//   single sensor, no mux       : { {0, -1} }
//   two sensors, two buses      : { {0, -1}, {1, -1} }
//   four sensors on a TCA9548A  : { {0, 0}, {0, 1}, {0, 2}, {0, 3} }
// ---------------------------------------------------------------------------
struct SensorSlot {
  uint8_t bus;
  int8_t muxChannel;
};

constexpr SensorSlot SENSOR_SLOTS[] = {
    {0, -1},
};
constexpr size_t NUM_SENSORS = sizeof(SENSOR_SLOTS) / sizeof(SENSOR_SLOTS[0]);

// ---------------------------------------------------------------------------
// MAX30105 acquisition settings (shared by all sensors)
// ---------------------------------------------------------------------------
// Works with MAX30102 (red + IR) and MAX30105 (red + IR + green). Both chips
// report the same part ID, so the firmware cannot tell them apart: pick here.
// LED_MODE: 1 = red only, 2 = red + IR (MAX30102 / MAX30105),
//           3 = red + IR + green (MAX30105 only; a MAX30102 has no green LED)
constexpr uint8_t LED_MODE = 2;
// Internal ADC sample rate: 50, 100, 200, 400, 800, 1000, 1600, 3200
constexpr int SAMPLE_RATE = 200;
// On-chip averaging: 1, 2, 4, 8, 16, 32. Output rate = SAMPLE_RATE / SAMPLE_AVG
constexpr uint8_t SAMPLE_AVG = 1;
// LED pulse width in us -> ADC resolution: 69 (15 bit), 118 (16), 215 (17), 411 (18)
// Long pulse widths limit the max sample rate (datasheet table). With red + IR,
// 411 us works up to 400 Hz; with 3 LEDs keep 411 us at <= 200 Hz.
constexpr int PULSE_WIDTH_US = 411;
// ADC full scale in nA: 2048, 4096, 8192, 16384
constexpr int ADC_RANGE = 16384;

// Initial LED currents, 0..255 (0x00 = 0 mA, 0xFF = 50 mA).
// Adjust at runtime with the "led" command so DC sits at ~50-80% of full scale.
constexpr uint8_t LED_RED_AMP = 0x1F;
constexpr uint8_t LED_IR_AMP = 0x1F;
constexpr uint8_t LED_GREEN_AMP = 0x1F;  // ignored unless LED_MODE == 3

// Start streaming immediately after boot (otherwise wait for "start")
constexpr bool AUTOSTART = true;
