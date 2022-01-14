## Controller for PCB preheater soldering station.
Main CPU: STM32F103C8

Display: OLED 12864

Input: 3 thermocouple K-type

Output: 2 dimmable channel


PCB software: diptrace 4

FW software: CubeIDE 1.7.0

![Front](/docs/img/front.png)
![Back](/docs/img/back.png)
![Assembly](/docs/img/ass.png)

Extension ports:
- 1 SPI: 3x MAX6675(in development)/MAX31856(tested, ok) board
- 2 I2C: MLX90614 (in development)
- 1Wire: 18b20 (tested, ok)
- 2-color led (tested, ok)
- 3.3v active buzzer (tested, ok)
- Power out (for fan)

Remote over uart:
- HC-05 bluetooth board (tested, ok)
- ESP12F WIFI board (firmware in development)
- CH340G USB Optoisolated board (need to be tested)

Clients:
- Flutter application (android only now)
- Any your modbus client (115200, 8/1, Even)

TODO:
- Linearization for thermocouple over ADC (integrated thermocouple input)
- Documentation
