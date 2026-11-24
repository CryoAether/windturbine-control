#include "anemometer.hpp"

#define WIND_PIN 27
static constexpr float MS_PER_HZ = 0.667f; // Calibration factor (e.g., 0.667 for Misol/WH-SP-WS01, 1.006 for Davis)
static constexpr unsigned long DEBOUNCE_US = 3000;      // 3 ms debounce (~333 Hz max)
static constexpr unsigned long SAMPLE_WINDOW_MS = 1000; // 1-second measurement window

static volatile unsigned long pulses = 0;
static volatile unsigned long lastPulseUs = 0;
static portMUX_TYPE pulseMux = portMUX_INITIALIZER_UNLOCKED;

static float cached_wind_ms = 0.0f;
static unsigned long last_calc_ms = 0;

void IRAM_ATTR onPulse() {
    unsigned long now = micros();
    if (now - lastPulseUs > DEBOUNCE_US) {
        portENTER_CRITICAL_ISR(&pulseMux);
        pulses++;
        portEXIT_CRITICAL_ISR(&pulseMux);
        lastPulseUs = now;
    }
}

void setup_anemometer() {
    pinMode(WIND_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(WIND_PIN), onPulse, RISING);
    last_calc_ms = millis();
}

void update_anemometer() {
    unsigned long now = millis();
    unsigned long elapsed_ms = now - last_calc_ms;

    if (elapsed_ms >= SAMPLE_WINDOW_MS) {
        portENTER_CRITICAL(&pulseMux);
        unsigned long p = pulses;
        pulses = 0;
        portEXIT_CRITICAL(&pulseMux);

        last_calc_ms = now;

        float hz = (float)p / (elapsed_ms / 1000.0f);
        cached_wind_ms = hz * MS_PER_HZ;
    }
}

float read_wind_speed_ms() {
    return cached_wind_ms;
}

float read_wind_speed_kmh() {
    return cached_wind_ms * 3.6f;
}