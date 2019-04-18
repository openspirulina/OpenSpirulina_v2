/**
 * OpenSpirulina http://www.openspirulina.com
 *
 * Autors: Sergio Arroyo (UOC)
 * 
 * General methods and functions related to the optical density sensor,
 * designed by OpenSpirulina
 * 
 */

#include "DO_Sensor.h"


DO_Sensor::DO_Sensor() {
    n_samples    = DO_SENS_N_SAMP_READ;
    ms_reads     = DO_SENS_MS_READS;
    RGBW_results = {0, 0, 0, 0};
    initialized  = false;
}

bool DO_Sensor::begin(uint8_t _addr, uint8_t _R_pin, uint8_t _G_pin, uint8_t _B_pin) {
    if (initialized) return true;                          // If the sensor already started, return true and not tray more

    // LEDs pins assign
    R_pin = _R_pin;
    G_pin = _G_pin;
    B_pin = _B_pin;
    pinMode(R_pin, OUTPUT);
    pinMode(G_pin, OUTPUT);
    pinMode(B_pin, OUTPUT);

    bh1750_dev = new BH1750(_addr);                        // Instanciate new BH1750 object

    // If not initialized exit and return false
    if (!bh1750_dev->begin(BH1750::Mode::CONTINUOUS_HIGH_RES_MODE_2))
        return false;                                      // Measurement at 0.5 lux resolution. Measurement time is approx 120ms.
    
    initialized = true;
    return true;
}

/* Capture DO values */
void DO_Sensor::capture_DO() {
    RGBW_results.R_value = capture_Red_LED();              // Get the values for each LED color from the DO
    RGBW_results.G_value = capture_Green_LED();
    RGBW_results.B_value = capture_Blue_LED();
    RGBW_results.W_value = capture_White_LED();
}

/* Capture light value without any led activated */
const float DO_Sensor::readLightLevel() {
    return bh1750_dev->readLightLevel();
}

/* Red light values for DO */
const float DO_Sensor::capture_Red_LED() {
    digitalWrite(R_pin, HIGH);
    delay(500);

    float iir[n_samples];
    for (uint8_t i=0; i<n_samples; i++) {
        iir[i] = bh1750_dev->readLightLevel();             // Read the BH1750 lux value
        delay(ms_reads);
    }
    digitalWrite(R_pin, LOW);

    return filter_result(iir, n_samples);
}

/* Green light values for DO */
const float DO_Sensor::capture_Green_LED() {
    digitalWrite(G_pin, HIGH);
    delay(500);

    float iir[n_samples];
    for (uint8_t i=0; i<n_samples; i++) {
        iir[i] = bh1750_dev->readLightLevel();
        delay(ms_reads);
    }

    digitalWrite(G_pin, LOW);    
    return filter_result(iir, n_samples);
}

/* Blue light values for DO */
const float DO_Sensor::capture_Blue_LED() {
    digitalWrite(B_pin, HIGH);
    delay(500);

    float iir[n_samples];
    for (uint8_t i=0; i<n_samples; i++) {
        iir[i] = bh1750_dev->readLightLevel();
        delay(ms_reads);
    }

    digitalWrite(B_pin, LOW);
    return filter_result(iir, n_samples);
}

/* White light values for DO */
const float DO_Sensor::capture_White_LED() {
    digitalWrite(R_pin, HIGH);
    digitalWrite(G_pin, HIGH);
    digitalWrite(B_pin, HIGH);
    delay(500);

    float iir[n_samples];
    for (uint8_t i=0; i<n_samples; i++) {
        iir[i] = bh1750_dev->readLightLevel();
        delay(ms_reads);
    }

    digitalWrite(R_pin, LOW);
    digitalWrite(G_pin, LOW);
    digitalWrite(B_pin, LOW);
    return filter_result(iir, n_samples);
}

const float DO_Sensor::get_Red_value() {
    return RGBW_results.R_value;
}

const float DO_Sensor::get_Green_value() {
    return RGBW_results.G_value;
}

const float DO_Sensor::get_Blue_value() {
    return RGBW_results.B_value;
}

const float DO_Sensor::get_White_value() {
    return RGBW_results.W_value;
}

/* Sort a list of value items and calculate the efective value */
float DO_Sensor::filter_result(float* list, uint8_t n_samples) {
    sort_result(list, n_samples);
    
    // Calculate the final result
    float sum_values = 0;
    for (uint8_t r=1; r < (n_samples-1); r++)                        // Sums the values except the first and last elements
        sum_values += list[r];
    
    return (sum_values / (n_samples-2));
}

void DO_Sensor::set_n_samples(const uint8_t _n_samples) {
    n_samples = _n_samples;
}

uint8_t DO_Sensor::get_n_samples() {
    return n_samples;
}

void DO_Sensor::set_ms_reads(const uint16_t _ms_reads) {
    ms_reads = _ms_reads;
}

uint16_t DO_Sensor::get_ms_reads() {
    return ms_reads;
}

bool DO_Sensor::is_init() {
    return initialized;
}

void DO_Sensor::sort_result(float* arr, const uint8_t n_samples) {
    float tmp_val;

    for (int i=0; i < (n_samples-1); i++) {
        for (int j=0; j < (n_samples-(i+1)); j++) {
            if (arr[j] > arr[j+1]) {
                tmp_val = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp_val;
            }
        }
    }
}
