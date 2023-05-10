# IVC_Tracer_v3

Source Measurement Unit (SMU).

����� ���������� � ����������� ������ ���������� ��� ������ ������ �����-�������� ������������� ����������������� ����������������.

## ���������� ����������������

������������ ���������� ��������� ������� �� ���� ���������������� ATmega2560.

FUSE �����: E:FD, H:DA, L:FF (������������� ������� ��������� ��������� �� 16 ���)

������� FUSE �����:
```
avrdude -c usbasp -p m2560 -B8 -U hfuse:w:0xDA:m
avrdude -c usbasp -p m2560 -B8 -U lfuse:w:0xE2:m
avrdude -c usbasp -p m2560 -B8 -U efuse:w:0xFE:m
```

�������� UART 115200 ���.

��������� ���������
```
avrdude -c usbasp -p m2560 -B8 -U flash:w:optiboot_flash_atmegam2560_UART0_115200_16000000L_B5.hex
```

���������� �������� �������� ����� UART:
```
avrdude -c wiring -p m2560 -b 115200 -P COM3 -U flash:w:rom.hex
```
