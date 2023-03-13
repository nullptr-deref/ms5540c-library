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
For further explanations on the comments read section
"Explanations" below.

```ino
#include <SPI.h>
#include <ms5540c.h>

ms5540c sensor;

void setup() {
    Serial.begin(9600);
    SPI.begin();
    sensor.init(); // Vital first-run setup
}

void loop() {
    const long temp = sensor.getTemperature(); // Temperature in tenths of the deg C
    const long prs = sensor.getPressure(); // pressure in tenths of a mbar (because of the sensor precision)
    Serial.print("Temp: ");
    Serial.println(degC(temp)); // degC() is library-defined conversion
    Serial.print("\tPressure: ");
    Serial.println(mbarTommHg(10*prs));

    delay(3000);
}
```
### Explanations

```ino
SPI.begin(<desired_baud_rate>);
```

Library doesn't start SPI internally so you should
start SPI connection yourself.

```ino
const long temp = sensor.getTemperature(); // Temperature in tenths of the deg C
const long prs = sensor.getPressure(); // pressure in tenths of a mbar (because of the sensor precision)
```

Because the accuracy of the sensor is one-tenth
of the mbar and one-tenth of the degrees Celsius,
values of the temperature and pressure are being
returned in tenths of mbars and deg's C respectively.
My library does not execute any conversions on the
raw data and transfers all of the conversions to
the user-defined code.
This is done in such way because AVR MCU's (at least,
those MCU's that are used in Arduino devices) does
not support floating-point in hardware.
Still, I provide handy functions that provide
handy conversions between pressure units and
convert raw temperature data to degrees Celsius.

## Connection with MCU features

Library uses hardware implementation of SPI library, so refer to actual pinout diagram of
your MCU to make sure which actual pins are reliable for MISO/MOSI/SCK lines.

**Very important note**: sensor lacks internal clock source so it is needed to provide external
clock source. It is made via MCLK pin. Make sure you've connected this input to `OC1A` pin
(refer to your actual MCU and Arduino pinout). Connecting this input to some another pin
will lead to sensor malfunctioning and you'll get strange and incorrect measurement
results (like negative pressure, etc.).

Note: it's better to have pullup resistors on serial lines to get rid of noise.
