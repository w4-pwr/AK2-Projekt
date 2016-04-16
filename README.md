# AK2Projekt

Główny kod aplikacji znajudje się w `lcd.c`

### Schemat podłączenia

![schemat](http://i.imgur.com/yArQZMv.png)

### Użyte piny

```
#define LCD_RS  3               //Register select pin
#define LCD_E   0               //Enable Pin
#define LCD_D4  6               //Data pin 4
#define LCD_D5  1               //Data pin 5
#define LCD_D6  5               //Data pin 6
#define LCD_D7  4               //Data pin 7
```

### Podłączenie GPS

```
//RPI-------------->GPS
//3.3V------------->Vcc
//RX--------------->Tx
//TX--------------->Rx
//GND-------------->Gnd
```

po poprawnym podłączneiu jesteśmy w stanie odczytywać pozycję z GPS za pomocą komendy 

`sudo cat /dev/ttyAMA0`

![output](http://i.imgur.com/rgA8bmV.png)

### Kompilacja i uruchomienie
```
gcc lcd.c -o lcd -lwiringPi -lwiringPiDev -lgps -lm
./lcd
```

### Przydatne linki

1. http://walterdalmut.com/2013/11/23/raspberry-pi-c-gps-nmea-library-global-positioning-system/
2. https://github.com/wdalmut/libgps
3. http://stackoverflow.com/questions/32000328/libgps-c-code-example
