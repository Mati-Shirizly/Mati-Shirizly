// ESP32 + MAX30105: raw red / IR / green streamer for PPG-based motion sensing.
//
// Output (CSV over USB serial, lines starting with '#' are status/comments):
//   sensor,seq,t_us,red,ir,green
//     sensor : index into SENSOR_SLOTS
//     seq    : per-sensor sample counter. Gaps = samples lost (FIFO overflow).
//              Use seq / OUTPUT_RATE_HZ as the authoritative time base.
//     t_us   : ESP32 micros() estimate of the sample time (for alignment only)
//
// Commands (newline terminated):
//   start | stop                 start / stop streaming
//   info                         print configuration and sensor status
//   led <s|*> <red> <ir> <green> set LED currents 0..255 for sensor s or all
//   temp                         print die temperature of each sensor
//   reset                        re-initialise all sensors

#include <Arduino.h>
#include <Wire.h>

#include "config.h"
#include "max30105.h"

static_assert(NUM_SENSORS >= 1, "Define at least one sensor in SENSOR_SLOTS");
constexpr bool slotsValid(size_t i = 0) {
  return i >= NUM_SENSORS ||
         (SENSOR_SLOTS[i].bus < 2 && SENSOR_SLOTS[i].muxChannel >= -1 &&
          SENSOR_SLOTS[i].muxChannel < 8 && slotsValid(i + 1));
}
static_assert(slotsValid(), "SENSOR_SLOTS: bus must be 0/1, muxChannel -1..7");
static_assert(LED_MODE >= 1 && LED_MODE <= 3, "LED_MODE must be 1, 2 or 3");

constexpr float OUTPUT_RATE_HZ = float(SAMPLE_RATE) / SAMPLE_AVG;
constexpr uint32_t SAMPLE_PERIOD_US = uint32_t(1e6f / OUTPUT_RATE_HZ);

struct SensorState {
  Max30105 dev;
  bool ok = false;
  uint32_t seq = 0;
  uint32_t lostTotal = 0;
  uint32_t lastReadUs = 0;
};

static SensorState sensors[NUM_SENSORS];
static TwoWire *buses[2] = {&Wire, &Wire1};
static int8_t currentMuxChannel[2] = {-2, -2};  // -2 = unknown
static bool busHasMux[2] = {false, false};
static bool streaming = AUTOSTART;

// ---------------------------------------------------------------------------
// I2C multiplexer handling
// ---------------------------------------------------------------------------
static bool selectSlot(size_t i) {
  const SensorSlot &slot = SENSOR_SLOTS[i];
  if (!busHasMux[slot.bus]) return true;
  if (currentMuxChannel[slot.bus] == slot.muxChannel) return true;
  TwoWire *w = buses[slot.bus];
  w->beginTransmission(TCA9548A_ADDR);
  // A direct (-1) sensor on a bus that also has a mux: disable all channels.
  w->write(slot.muxChannel >= 0 ? uint8_t(1u << slot.muxChannel) : uint8_t(0));
  bool ok = w->endTransmission() == 0;
  currentMuxChannel[slot.bus] = ok ? slot.muxChannel : -2;
  return ok;
}

// ---------------------------------------------------------------------------
// Sensor setup
// ---------------------------------------------------------------------------
static Max30105::Settings settings() {
  return {LED_MODE,  SAMPLE_RATE, SAMPLE_AVG,   PULSE_WIDTH_US, ADC_RANGE,
          LED_RED_AMP, LED_IR_AMP, LED_GREEN_AMP};
}

static void initSensors() {
  for (size_t i = 0; i < NUM_SENSORS; ++i) {
    SensorState &s = sensors[i];
    s.ok = selectSlot(i) && s.dev.begin(*buses[SENSOR_SLOTS[i].bus]) &&
           s.dev.configure(settings());
    s.seq = 0;
    s.lostTotal = 0;
    s.lastReadUs = micros();
    Serial.printf("# sensor %u bus=%u mux=%d : %s\n", unsigned(i), SENSOR_SLOTS[i].bus,
                  SENSOR_SLOTS[i].muxChannel, s.ok ? "OK" : "NOT FOUND / CONFIG FAILED");
  }
}

static void printInfo() {
  Serial.printf("# ppg-motion firmware | sensors=%u led_mode=%u sr=%d avg=%u pw=%dus "
                "range=%dnA out_rate=%.1fHz streaming=%d\n",
                unsigned(NUM_SENSORS), LED_MODE, SAMPLE_RATE, SAMPLE_AVG, PULSE_WIDTH_US,
                ADC_RANGE, OUTPUT_RATE_HZ, streaming);
  for (size_t i = 0; i < NUM_SENSORS; ++i)
    Serial.printf("# sensor %u ok=%d seq=%lu lost=%lu\n", unsigned(i), sensors[i].ok,
                  (unsigned long)sensors[i].seq, (unsigned long)sensors[i].lostTotal);
  Serial.println("# columns: sensor,seq,t_us,red,ir,green");
}

// ---------------------------------------------------------------------------
// Serial command parser
// ---------------------------------------------------------------------------
static void handleCommand(char *line) {
  char *cmd = strtok(line, " \t");
  if (!cmd) return;

  if (!strcmp(cmd, "start")) {
    streaming = true;
    Serial.println("# columns: sensor,seq,t_us,red,ir,green");
  } else if (!strcmp(cmd, "stop")) {
    streaming = false;
    Serial.println("# stopped");
  } else if (!strcmp(cmd, "info")) {
    printInfo();
  } else if (!strcmp(cmd, "reset")) {
    initSensors();
  } else if (!strcmp(cmd, "temp")) {
    for (size_t i = 0; i < NUM_SENSORS; ++i) {
      if (!sensors[i].ok || !selectSlot(i)) continue;
      Serial.printf("# temp sensor=%u %.2fC\n", unsigned(i), sensors[i].dev.readTemperatureC());
    }
  } else if (!strcmp(cmd, "led")) {
    char *who = strtok(nullptr, " \t");
    char *r = strtok(nullptr, " \t");
    char *ir = strtok(nullptr, " \t");
    char *g = strtok(nullptr, " \t");
    if (!who || !r || !ir || !g) {
      Serial.println("# usage: led <sensor|*> <red> <ir> <green>  (0..255)");
      return;
    }
    bool all = !strcmp(who, "*");
    long idx = all ? -1 : strtol(who, nullptr, 10);
    uint8_t vr = constrain(atoi(r), 0, 255), vir = constrain(atoi(ir), 0, 255),
            vg = constrain(atoi(g), 0, 255);
    for (size_t i = 0; i < NUM_SENSORS; ++i) {
      if (!(all || long(i) == idx) || !sensors[i].ok || !selectSlot(i)) continue;
      sensors[i].dev.setLedAmplitudes(vr, vir, vg);
      Serial.printf("# led sensor=%u red=%u ir=%u green=%u\n", unsigned(i), vr, vir, vg);
    }
  } else {
    Serial.printf("# unknown command '%s' (start|stop|info|led|temp|reset)\n", cmd);
  }
}

static void pollSerial() {
  static char buf[64];
  static size_t len = 0;
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\r') continue;
    if (c == '\n') {
      buf[len] = '\0';
      handleCommand(buf);
      len = 0;
    } else if (len < sizeof(buf) - 1) {
      buf[len++] = c;
    }
  }
}

// ---------------------------------------------------------------------------
// Acquisition
// ---------------------------------------------------------------------------
static void drainSensor(size_t i) {
  SensorState &s = sensors[i];
  if (!s.ok || !selectSlot(i)) return;

  static Max30105::Sample samples[Max30105::FIFO_DEPTH];
  uint8_t lost = 0;
  uint8_t n = s.dev.readFifo(samples, lost);
  uint32_t tRead = micros();
  uint32_t dropped = lost;
  // The hardware overflow counter saturates at 31; after a longer stall
  // estimate the real loss from the elapsed time instead.
  if (lost >= 31) {
    uint32_t expected = (tRead - s.lastReadUs + SAMPLE_PERIOD_US / 2) / SAMPLE_PERIOD_US;
    if (expected > n + dropped) dropped = expected - n;
  }
  s.lastReadUs = tRead;

  if (dropped) {
    s.seq += dropped;  // keep seq aligned with real time so the gap is visible
    s.lostTotal += dropped;
    if (streaming)
      Serial.printf("# overflow sensor=%u lost=%lu%s\n", unsigned(i), (unsigned long)dropped,
                    lost >= 31 ? " (estimated)" : "");
  }
  if (!streaming) {
    s.seq += n;
    return;
  }

  // One buffered write per FIFO burst keeps the serial overhead low.
  static char out[Max30105::FIFO_DEPTH * 48];
  size_t pos = 0;
  for (uint8_t k = 0; k < n; ++k) {
    uint32_t t = tRead - uint32_t(n - 1 - k) * SAMPLE_PERIOD_US;
    pos += snprintf(out + pos, sizeof(out) - pos, "%u,%lu,%lu,%lu,%lu,%lu\n", unsigned(i),
                    (unsigned long)s.seq++, (unsigned long)t, (unsigned long)samples[k].red,
                    (unsigned long)samples[k].ir, (unsigned long)samples[k].green);
  }
  Serial.write(reinterpret_cast<const uint8_t *>(out), pos);
}

void setup() {
  Serial.setTxBufferSize(8192);
  Serial.begin(SERIAL_BAUD);
  delay(200);

  bool busUsed[2] = {false, false};
  for (size_t i = 0; i < NUM_SENSORS; ++i) {
    busUsed[SENSOR_SLOTS[i].bus] = true;
    if (SENSOR_SLOTS[i].muxChannel >= 0) busHasMux[SENSOR_SLOTS[i].bus] = true;
  }
  if (busUsed[0]) Wire.begin(I2C0_SDA, I2C0_SCL, I2C_CLOCK_HZ);
  if (busUsed[1]) Wire1.begin(I2C1_SDA, I2C1_SCL, I2C_CLOCK_HZ);

  initSensors();
  printInfo();
}

void loop() {
  pollSerial();
  for (size_t i = 0; i < NUM_SENSORS; ++i) drainSensor(i);
  // The FIFO holds 32 samples (160 ms at 200 Hz), so a short yield is safe.
  delay(1);
}
