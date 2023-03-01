#ifndef MS5540C_H
#define MS5540C_H

#include <SPI.h>

// === READ THIS SECTION BEFORE PERFORMING ANY ACTIONS WITH MS5540C SENSOR! ===
// First of all, I should mention that device, for which this library was written,
// communicates with it's master controller via SPI protocol, so you should
// consult Arduino pinout diagram (https://docs.arduino.cc/hardware/uno-rev3).
// For general Arduino Uno R3 device required pins are follow:
// MOSI (COPI) - D11
// MISO (CIPO) - D12
// SCK - D13
// ... and, for some reason (I haven't investigated it yet) we should use MCLK as well.
// It is obligatory, without attaching this pin somewhere (pin which is used for it
// is configured via ctor of ms5540c class) measurement acquisition won't work.

const int CONV_DUR = 35;

const byte RST_SEQ[3] = {
    0x15,
    0x55,
    0x40
};

const int16_t WORDS_ACQ[8] = {
    0x1D, 0x50, // 1st word
    0x1D, 0x60, // 2nd word
    0x1D, 0x90, // 3rd word
    0x1D, 0xA0  // 4th word
};

enum MeasurementType {
    Temperature,
    Pressure
};

enum SecondOrderCompensation {
    SOC = true,
    noSOC = false
};

const byte TMP_MSR[2] = {
    0x0F,
    0x20
};

const byte PRS_MSR[2] = {
    0x0F,
    0x40
};

enum UnitType {
    mbar,
    mmHg
};

class ms5540c {
public:
    ms5540c(int mclk);

    void init();
    void reset() const;
    float getTemperature(SecondOrderCompensation secondOrder = SOC);
    float getPressure(UnitType t, SecondOrderCompensation secondOrder = noSOC);

private:
    int16_t readWord(int widx);
    int16_t readData(MeasurementType t) const;
    long getTempi() const;
    long getPressurei() const;
    long calcRefAndActualTempDifference() const;

    int mclk_;
    long coefs[6];
};

#endif
