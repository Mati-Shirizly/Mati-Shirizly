#include "max30105.h"

#include <algorithm>

namespace {
// Register map (MAX30105 datasheet)
constexpr uint8_t REG_FIFO_WR_PTR = 0x04;
constexpr uint8_t REG_OVF_COUNTER = 0x05;
constexpr uint8_t REG_FIFO_RD_PTR = 0x06;
constexpr uint8_t REG_FIFO_DATA = 0x07;
constexpr uint8_t REG_FIFO_CONFIG = 0x08;
constexpr uint8_t REG_MODE_CONFIG = 0x09;
constexpr uint8_t REG_PARTICLE_CONFIG = 0x0A;
constexpr uint8_t REG_LED1_PA = 0x0C;  // red
constexpr uint8_t REG_LED2_PA = 0x0D;  // IR
constexpr uint8_t REG_LED3_PA = 0x0E;  // green
constexpr uint8_t REG_MULTI_LED_1 = 0x11;  // slot 2 [6:4], slot 1 [2:0]
constexpr uint8_t REG_MULTI_LED_2 = 0x12;  // slot 4 [6:4], slot 3 [2:0]
constexpr uint8_t REG_TEMP_INT = 0x1F;
constexpr uint8_t REG_TEMP_FRAC = 0x20;
constexpr uint8_t REG_TEMP_CONFIG = 0x21;
constexpr uint8_t REG_PART_ID = 0xFF;

constexpr uint8_t MODE_SHDN = 0x80;
constexpr uint8_t MODE_RESET = 0x40;
constexpr uint8_t MODE_MULTI_LED = 0x07;
constexpr uint8_t FIFO_ROLLOVER_EN = 0x10;

constexpr uint8_t SLOT_RED = 1, SLOT_IR = 2, SLOT_GREEN = 3;

int indexOf(const int *table, size_t n, int value) {
  for (size_t i = 0; i < n; ++i)
    if (table[i] == value) return static_cast<int>(i);
  return -1;
}
}  // namespace

bool Max30105::begin(TwoWire &wire) {
  wire_ = &wire;
  uint8_t id = 0;
  return read8(REG_PART_ID, id) && id == PART_ID;
}

bool Max30105::configure(const Settings &s) {
  static const int kAvg[] = {1, 2, 4, 8, 16, 32};
  static const int kRate[] = {50, 100, 200, 400, 800, 1000, 1600, 3200};
  static const int kPw[] = {69, 118, 215, 411};
  static const int kRange[] = {2048, 4096, 8192, 16384};

  int avg = indexOf(kAvg, 6, s.sampleAvg);
  int rate = indexOf(kRate, 8, s.sampleRate);
  int pw = indexOf(kPw, 4, s.pulseWidthUs);
  int range = indexOf(kRange, 4, s.adcRangeNa);
  if (avg < 0 || rate < 0 || pw < 0 || range < 0 || s.ledMode < 1 || s.ledMode > 3)
    return false;

  // Soft reset and wait for the chip to clear the RESET bit.
  if (!write8(REG_MODE_CONFIG, MODE_RESET)) return false;
  uint32_t t0 = millis();
  uint8_t mode = MODE_RESET;
  while ((mode & MODE_RESET) && millis() - t0 < 100) {
    if (!read8(REG_MODE_CONFIG, mode)) return false;
    delay(1);
  }
  if (mode & MODE_RESET) return false;

  // FIFO: averaging + rollover so the newest data is kept; OVF counts losses.
  write8(REG_FIFO_CONFIG, (avg << 5) | FIFO_ROLLOVER_EN);
  write8(REG_PARTICLE_CONFIG, (range << 5) | (rate << 2) | pw);
  setLedAmplitudes(s.redAmp, s.irAmp, s.greenAmp);

  // Multi-LED mode with explicit time slots keeps the FIFO order fixed:
  // red, IR, green (only the first `ledMode` slots are enabled).
  activeLeds_ = s.ledMode;
  uint8_t slot2 = s.ledMode >= 2 ? SLOT_IR : 0;
  uint8_t slot3 = s.ledMode >= 3 ? SLOT_GREEN : 0;
  write8(REG_MULTI_LED_1, (slot2 << 4) | SLOT_RED);
  write8(REG_MULTI_LED_2, slot3);
  write8(REG_MODE_CONFIG, MODE_MULTI_LED);

  // Start with an empty FIFO.
  write8(REG_FIFO_WR_PTR, 0);
  write8(REG_OVF_COUNTER, 0);
  return write8(REG_FIFO_RD_PTR, 0);
}

void Max30105::setLedAmplitudes(uint8_t red, uint8_t ir, uint8_t green) {
  write8(REG_LED1_PA, red);
  write8(REG_LED2_PA, ir);
  write8(REG_LED3_PA, green);
}

void Max30105::shutdown(bool off) {
  update8(REG_MODE_CONFIG, MODE_SHDN, off ? MODE_SHDN : 0);
}

float Max30105::readTemperatureC() {
  write8(REG_TEMP_CONFIG, 0x01);
  uint32_t t0 = millis();
  uint8_t busy = 1;
  while (busy && millis() - t0 < 100) {
    if (!read8(REG_TEMP_CONFIG, busy)) return NAN;
    busy &= 0x01;
    delay(1);
  }
  uint8_t ti = 0, tf = 0;
  if (busy || !read8(REG_TEMP_INT, ti) || !read8(REG_TEMP_FRAC, tf)) return NAN;
  return static_cast<int8_t>(ti) + (tf & 0x0F) * 0.0625f;
}

uint8_t Max30105::readFifo(Sample *out, uint8_t &lost) {
  lost = 0;
  uint8_t wr = 0, rd = 0, ovf = 0;
  // OVF_COUNTER resets when a sample is popped, so read it before the data.
  if (!read8(REG_OVF_COUNTER, ovf) || !read8(REG_FIFO_WR_PTR, wr) ||
      !read8(REG_FIFO_RD_PTR, rd))
    return 0;
  lost = ovf & 0x1F;

  uint8_t n = (wr - rd) & 0x1F;
  if (n == 0 && lost > 0) n = FIFO_DEPTH;  // pointers equal because FIFO is full
  if (n == 0 || activeLeds_ == 0) return 0;

  const uint8_t bytesPerSample = activeLeds_ * 3;
  // ESP32 Wire buffer is 128 bytes; read whole samples per transaction.
  const uint8_t samplesPerChunk = 120 / bytesPerSample;
  uint8_t done = 0;
  while (done < n) {
    uint8_t chunk = std::min<uint8_t>(samplesPerChunk, n - done);
    wire_->beginTransmission(I2C_ADDR);
    wire_->write(REG_FIFO_DATA);
    if (wire_->endTransmission(false) != 0) break;
    size_t len = chunk * bytesPerSample;
    if (wire_->requestFrom(static_cast<int>(I2C_ADDR), static_cast<int>(len)) != len) break;
    for (uint8_t i = 0; i < chunk; ++i) {
      uint32_t v[3] = {0, 0, 0};
      for (uint8_t led = 0; led < activeLeds_; ++led) {
        uint32_t b2 = wire_->read(), b1 = wire_->read(), b0 = wire_->read();
        v[led] = ((b2 << 16) | (b1 << 8) | b0) & 0x3FFFF;  // 18-bit, left-justified
      }
      out[done + i] = {v[0], v[1], v[2]};
    }
    done += chunk;
  }
  return done;
}

bool Max30105::write8(uint8_t reg, uint8_t val) {
  wire_->beginTransmission(I2C_ADDR);
  wire_->write(reg);
  wire_->write(val);
  return wire_->endTransmission() == 0;
}

bool Max30105::read8(uint8_t reg, uint8_t &val) {
  wire_->beginTransmission(I2C_ADDR);
  wire_->write(reg);
  if (wire_->endTransmission(false) != 0) return false;
  if (wire_->requestFrom(static_cast<int>(I2C_ADDR), 1) != 1) return false;
  val = wire_->read();
  return true;
}

bool Max30105::update8(uint8_t reg, uint8_t mask, uint8_t bits) {
  uint8_t v = 0;
  if (!read8(reg, v)) return false;
  return write8(reg, (v & ~mask) | (bits & mask));
}
