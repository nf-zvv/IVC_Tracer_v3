# IVC_Tracer_v3

Source Measurement Unit (SMU).

Новая аппаратная и программная версия устройства для снятия полных вольт-амперных характеристик фотоэлектрических преобразователей.

## Подготовка микроконтроллера

Используется аппаратная платформа Магония на базе микроконтроллера ATmega2560.

FUSE байты: E:FD, H:DA, L:FF (использовался внешний кварцевый резонатор на 16 МГц)

Записал FUSE байты:
```
avrdude -c usbasp -p m2560 -B8 -U hfuse:w:0xDA:m
avrdude -c usbasp -p m2560 -B8 -U lfuse:w:0xE2:m
avrdude -c usbasp -p m2560 -B8 -U efuse:w:0xFE:m
```

Скорость UART 115200 бод.

Прошиваем бутлоадер
```
avrdude -c usbasp -p m2560 -B8 -U flash:w:optiboot_flash_atmegam2560_UART0_115200_16000000L_B5.hex
```

Дальнейшая прошивка возможна через UART:
```
avrdude -c wiring -p m2560 -b 115200 -P COM3 -U flash:w:rom.hex
```
