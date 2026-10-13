#pragma once
#include <Arduino.h>
#include <Wire.h>

// Minimal register-level MAX30105 driver focused on lossless raw streaming.
// Unlike the SparkFun library it drains the whole 32-sample hardware FIFO in
// one burst and reports how many samples the chip dropped (overflow counter).
class Max30105 {
 public:
  static constexpr uint8_t I2C_ADDR = 0x57;
  static constexpr uint8_t PART_ID = 0x15;
  static constexpr uint8_t FIFO_DEPTH = 32;

  struct Sample {
    uint32_t red;
    uint32_t ir;
    uint32_t green;
  };

  struct Settings {
    uint8_t ledMode;      // 1 = red, 2 = red+IR, 3 = red+IR+green
    int sampleRate;       // Hz: 50..3200
    uint8_t sampleAvg;    // 1..32
    int pulseWidthUs;     // 69, 118, 215, 411
    int adcRangeNa;       // 2048, 4096, 8192, 16384
    uint8_t redAmp, irAmp, greenAmp;
  };

  // Returns false if no MAX30105 answers or the part ID is wrong.
  bool begin(TwoWire &wire);
  // Soft-resets the chip and applies the settings. Returns false on bad settings.
  bool configure(const Settings &s);
  void setLedAmplitudes(uint8_t red, uint8_t ir, uint8_t green);
  void shutdown(bool off);
  float readTemperatureC();

  // Drains the FIFO into `out` (capacity FIFO_DEPTH). Returns number of samples
  // read; `lost` receives the number of samples the chip discarded since the
  // previous read (saturates at 31 in hardware).
  uint8_t readFifo(Sample *out, uint8_t &lost);

  uint8_t activeLeds() const { return activeLeds_; }

 private:
  bool write8(uint8_t reg, uint8_t val);
  bool read8(uint8_t reg, uint8_t &val);
  bool update8(uint8_t reg, uint8_t mask, uint8_t bits);

  TwoWire *wire_ = nullptr;
  uint8_t activeLeds_ = 0;
};
