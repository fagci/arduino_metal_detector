#ifndef MEASUREMENT_THREAD
#define MEASUREMENT_THREAD

#include "Thread.h"
#include <DIO2.h>
#include <avdweb_AnalogReadFast.h>

class MeasurementThread: public Thread {
  public:
    word charge_time_us;
    word delay_time_us;

    int* values;

    int min;
    int max;
    int trigger;
    const int count;

    MeasurementThread(const byte in_pin, const byte out_pin, const byte cnt);
    void run(void);
    void calibrate();
    void decalibrate();

  private:
    const byte input_pin;
    const byte output_pin;
    int* calibr;
};

#endif

