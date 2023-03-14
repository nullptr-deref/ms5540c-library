#include "ms5540c.h"

#include <Arduino.h>

#include <SPI.h>

float conv::mbarTommHg(long mbar) {
    return mbar * 750.06 / 100000;
}

float conv::degC(long temp) {
    return temp / 10.0f;
}

float conv::mbar(long pressure) {
    return pressure / 10.0f;
}

float conv::mbarToAtm(long mbar) {
    return mbar / 1013.25;
}

float conv::mbarToPascal(long mbar) {
    return mbar * 100;
}

void ms5540c::setupSPI() const {
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV32);
}

void ms5540c::begin() {
    this->setupSPI();
    pinMode(lib_int::MCLK, OUTPUT);
    TCCR1B = (TCCR1B & 0xF8) | 1;
    analogWrite(lib_int::MCLK, 128);
    this->reset();

    int16_t words[4];
    for (int i = 0; i < 4; i++) {
        this->reset();
        words[i] = readWord(i);
    }
    coefs[0] = (words[0] >> 1) & 0x7FFF;
    coefs[1] = ((words[2] & 0x003F) << 6) | (words[3] & 0x003F);
    coefs[2] = (words[3] >> 6) & 0x03FF;
    coefs[3] = (words[2] >> 6) & 0x03FF;
    coefs[4] = ((words[0] & 0x0001) << 10) | ((words[1] >> 6) & 0x03FF);
    coefs[5] = words[1] & 0x003F;
}

int16_t ms5540c::readWord(int8_t widx) const {
    setupSPI();
    byte recv[2];
    SPI.transfer(lib_int::WORDS_ACQ[widx*2]);
    SPI.transfer(lib_int::WORDS_ACQ[widx*2 + 1]);
    SPI.setDataMode(SPI_MODE1);
    recv[0] = SPI.transfer(0x0);
    recv[1] = SPI.transfer(0x0);

    return (recv[0] << 8) | recv[1];
}

int16_t ms5540c::readData(lib_int::MeasurementType t) const {
    setupSPI();
    this->reset();
    byte recv[2];
    switch(t) {
        case lib_int::MeasurementType::Temperature:
            SPI.transfer(lib_int::TMP_MSR[0]);
            SPI.transfer(lib_int::TMP_MSR[1]);
        break;
        case lib_int::MeasurementType::Pressure:
            SPI.transfer(lib_int::PRS_MSR[0]);
            SPI.transfer(lib_int::PRS_MSR[1]);
        break;
    }
    delay(lib_int::CONV_DUR);
    SPI.setDataMode(SPI_MODE1);
    recv[0] = SPI.transfer(0x0);
    recv[1] = SPI.transfer(0x0);

    return (recv[0] << 8) | recv[1];
}

void ms5540c::reset() const {
    setupSPI();
    SPI.setDataMode(SPI_MODE0);
    SPI.transfer(lib_int::RST_SEQ[0]);
    SPI.transfer(lib_int::RST_SEQ[1]);
    SPI.transfer(lib_int::RST_SEQ[2]);
}

long ms5540c::getTemperature(SecondOrderCompensation secondOrder) const {
    const long TEMP = getTempi();

    if (secondOrder) {
        long TEMP2 = TEMP;
        if (TEMP < 200 || TEMP > 450) {
            long T2 = 0;
            if (TEMP < 200) {
                T2 = (11 * (coefs[5] + 24) * (200 - TEMP) * (200 - TEMP) ) >> 20;
            }
            else {
                T2 = (3 * (coefs[5] + 24) * (450 - TEMP) * (450 - TEMP) ) >> 20;
            }
            TEMP2 = TEMP - T2;
        }

        return TEMP2;
    }

    return TEMP;
}

long ms5540c::getPressure(SecondOrderCompensation secondOrder) const {
    long PCOMP = getPressurei();

    if (secondOrder) {
        const long TEMP = getTempi();
        long PCOMP2 = PCOMP;
        if (TEMP < 200 || TEMP > 450) {
            long T2 = 0;
            long P2 = 0;
            if (TEMP < 200) {
                T2 = (11 * (coefs[5] + 24) * (200 - TEMP) * (200 - TEMP) ) >> 20;
                P2 = (3 * T2 * (PCOMP - 3500) ) >> 14;
            }
            else if (TEMP > 450) {
                T2 = (3 * (coefs[5] + 24) * (450 - TEMP) * (450 - TEMP) ) >> 20;
                P2 = (T2 * (PCOMP - 10000) ) >> 13;
            }
            PCOMP2 = PCOMP - P2;
        }

        return PCOMP2;
    }

    return PCOMP;
}

long ms5540c::calcRefAndActualTempDifference() const {
    const int16_t temp = readData(lib_int::MeasurementType::Temperature);
    const long UT1 = (coefs[4] << 3) + 20224;

    return temp - UT1;
}

long ms5540c::getTempi() const {
    const long dT = calcRefAndActualTempDifference();
    const long TEMP = 200 + ((dT * (coefs[5] + 50)) >> 10);

    return TEMP;
}

long ms5540c::getPressurei() const {
    const int16_t D1 = readData(lib_int::MeasurementType::Pressure);

    const long dT = calcRefAndActualTempDifference();

    const long OFF  = (coefs[1] * 4) + (((coefs[3] - 512) * dT) >> 12);
    const long SENS = coefs[0] + ((coefs[2] * dT) >> 10) + 24576;
    const long X = (SENS * (D1 - 7168) >> 14) - OFF;
    long PCOMP = ((X * 10) >> 5) + 2500;

    return PCOMP;
}
