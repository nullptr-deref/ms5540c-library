# MS5540C Arduino Library

**If you're looking for Russian version of this library, please visit** [it's Github page](https://github.com/militaryCoder/ms5540c-library-ru).

## Sensor description

Given sensor operates using slightly modified SPI interface (the main difference is that sensor does not
have SS/CS (Slave Select/Chip Select) line. Instead, this sensor takes 3 START and 3 STOP bits before and
after each bit control sequence.
It can operate up to 100m below water surface (as said in datasheet).

## Connection with MCU features

First of all, library uses hardware implementation of SPI library, so refer to actual pinout diagram of
your MCU to make sure which actual pins are reliable for MISO/MOSI/SCK lines.
Also, there is MCLK pin on the sensor so make sure to provide additional pin for MCLK line.
**Without the latter everything isn't going to work. You've been warned.**

Note: it's better to have pullup resistors on serial lines to get rid of noise.

## Code example

Here's a simple example which measures current temperature
and pressure in mbars each 3 seconds.

```ino
#include <ms5540c.h>

const int mclk = 13; // Actually can be any digital pin you want.
ms5540c sensor(mclk);

void setup() {
    Serial.begin(9600);
    SPI.begin();
    sensor.init();
}

void loop() {
    sensor.reset();
    const float temp = sensor.getTemperature();
    const float prs = sensor.getPressure(UnitType::mbar);
    Serial.print("Temp: ");
    Serial.println(temp);
    Serial.print("Pressure: ");
    Serial.println(prs);
    delay(3000);
}
```

### My actual pinout

| Line | Arduino pin |
|------|-------------|
| MOSI |     D11     |
| MISO |     D12     |
| SCK  |     D13     |
| SS   |     ---     |
