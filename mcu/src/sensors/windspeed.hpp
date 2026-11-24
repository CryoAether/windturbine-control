#ifndef ANEMOMETER_HPP
#define ANEMOMETER_HPP

#include <Arduino.h>

//  GPIO 27 interrupt
void setup_anemometer();

void update_anemometer();

float read_wind_speed_ms();
float read_wind_speed_kmh();

#endif // ANEMOMETER_HPP