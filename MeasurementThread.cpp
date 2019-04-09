#include "MeasurementThread.h"

MeasurementThread::MeasurementThread(const byte in_pin, const byte out_pin, const byte cnt):
  count(cnt),
  input_pin(in_pin),
  output_pin(out_pin) {
  values = new int[count];
  calibr = new int[count];
  pinMode2(input_pin, INPUT_PULLUP);
  pinMode2(output_pin, OUTPUT);
  decalibrate();
}

void MeasurementThread::run(void) {
  // Clear measurement data
  for (byte i = 0; i < count; i++) values[i] = 0;

  // Generate impulse with delay for measure
  digitalWrite2(output_pin, HIGH), delayMicroseconds(charge_time_us), digitalWrite2(output_pin, LOW), delayMicroseconds(delay_time_us);

  // Read data
  for (byte i = 0; i < count; i++) values[i] = analogReadFast(input_pin);
  for (byte i = 0; i < count; i++) values[i] -= calibr[i];

  min = max = values[0];

  for (byte i = 1; i < count; i++) {
    int value = values[i];
    min = min(value, min);
    max = max(value, max);
  }

  runned();
}

void MeasurementThread::calibrate() {
  for (byte i = 0; i < count; i++) calibr[i] = values[i];
}

void MeasurementThread::decalibrate() {
  for (byte i = 0; i < count; i++) calibr[i] = 0;
}

