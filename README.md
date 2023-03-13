# MS5540C Arduino Library

**If you're looking for Russian version of this library, please visit** [it's Github page](https://github.com/militaryCoder/ms5540c-library-ru).

## Brief sensor description

Given sensor operates using slightly modified SPI interface (the main difference is that sensor does not
have SS/CS (Slave Select/Chip Select) line. Instead, this sensor receives 3 START and 3 STOP bits before and
after each bit control sequence.
It can operate up to 100m below water surface (as said in datasheet).

## Code example

Here's a simple example which measures current temperature
and pressure in mbars each 3 seconds.
For further explanations on the comments read section
"Explanations" below.

*Also, if you don't know AVR MCU's like the back of your hand, please carefully read
"Important notes" section to avoid different tricky problems.*

```ino
#include <SPI.h>
#include <ms5540c.h>

ms5540c sensor;

void setup() {
    Serial.begin(9600);
    SPI.begin();
    sensor.init();
    Serial.println("Temp\t\tmbar\t\tatm\t\tpascal\n");
}

void loop() {
    const long temp_raw = sensor.getTemperature(); // Temperature in tenths of the deg C
    const long prs_raw = sensor.getPressure(); // pressure in tenths of a mbar (because of the sensor precision)
    Serial.print(conv::degC(temp_raw)); Serial.print("\t\t");
    Serial.print(conv::mbar(prs_raw)); Serial.print("\t\t");
    Serial.print(conv::mbarToAtm(conv::mbar(prs_raw))); Serial.print("\t\t");
    Serial.println(conv::mbarToPascal(conv::mbar(prs_raw)));

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
const long temp_raw = sensor.getTemperature(); // Temperature in tenths of the deg C
const long prs_raw = sensor.getPressure(); // pressure in tenths of a mbar (because of the sensor precision)
```
Because the accuracy of the sensor is one-tenth of the mbar and one-tenth of the degrees Celsius,
values of the temperature and pressure are being returned in tenths of mbars and deg's C respectively.

## Important notes

### What library does and doesn't do

This library retrieves information from ms5540c sensor "as is", i.e. without any
further complicated tranformations and calculations (except second-order temperature compensation).
Also, it doesn't support floating-point arithmetics because AVR MCU's (at least those
MCU's which are used in Arduino boards) don't support it on hardware level and that
creates a bit of a trouble while trying to convert, write or manipulate retrieved
data if it's returned as `float`'s directly from the library.
Still, I provide handy conversion function in the `conv` namespace which are briefly described below.

It is higly recommended to transfer floating-point operations to some PC-driven
scripts, program, etc.

### Connection with MCU features

Library uses hardware implementation of SPI library, so refer to actual pinout diagram of
your MCU to make sure which actual pins are reliable for MISO/MOSI/SCK lines.

**Very important note**: sensor lacks internal clock source so it is needed to provide external
clock source. It is made via MCLK pin. Make sure you've connected this input to `OC1A` pin
(refer to your actual MCU and Arduino pinout).

*Handy tip: if you're using Arduino Uno R3, `OC1A` is attached to pin D9.*

Connecting this input to some another pin will lead to sensor malfunctioning and you'll
get strange and incorrect measurement results (like negative pressure, etc.).

Note: it's better to have pullup resistors on serial lines to get rid of noise.

### Conversion functions

|    Function name                | What it does                             |
|---------------------------------|------------------------------------------|
| `conv::mbar(long prs_raw)`      | Converts raw pressure data into mbars    |
| `conv::degC(long temp_raw)`     | Converts raw temperature data into deg C |
| `conv::mbarTommHg(long mbar)`   | mbar -> mmHg                             |
| `conv::mbarToAtm(long mbar)`    | mbar -> atm                              |
| `conv::mbarToPascal(long mbar)` | mbar -> pascal                           |
