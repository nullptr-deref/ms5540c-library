# MS5540C Arduino Library

**If you're looking for Russian version of this library, please visit** [it's Github page](https://github.com/militaryCoder/ms5540c-library-ru).

## Sensor description

Given sensor operates using slightly modified SPI interface (the main difference is that sensor does not
have SS/CS (Slave Select/Chip Select) line. Instead, this sensor takes 3 START and 3 STOP bits before and
after each bit control sequence.
It can operate up to 100m below water surface (as said in datasheet).

## Code example

Here's a simple example which measures current temperature
and pressure in mbars each 3 seconds.

```ino
#include <SPI.h>
#include <ms5540c.h>

ms5540c sensor;

void setup() {
    Serial.begin(9600);
    SPI.begin();
    sensor.init();
}

void loop() {
    const float temp = sensor.getTemperature();
    const float prs = sensor.getPressure(UnitType::mbar);
    Serial.print("Temp: ");
    Serial.println(temp);
    Serial.print("\tPressure: ");
    Serial.println(prs);

    delay(3000);
}
```

## Connection with MCU features

Library uses hardware implementation of SPI library, so refer to actual pinout diagram of
your MCU to make sure which actual pins are reliable for MISO/MOSI/SCK lines.

**Very important note**: sensor lacks internal clock source so it is needed to provide external
clock source. It is made via MCLK pin. Make sure you've connected this input to `OC1A` pin
(refer to your actual MCU and Arduino pinout). Connecting this input to some another pin
will lead to sensor malfunctioning and you'll get strange and incorrect measurement
results (like negative pressure, etc.).

Note: it's better to have pullup resistors on serial lines to get rid of noise.
