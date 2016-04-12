# AK2Projekt

https://github.com/wdalmut/libgps

w lcd.c jest główny program

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

### Kompilacja i uruchomienie
```
gcc lcd.c -o lcd -lwiringPi -lwiringPiDev
./lcd
```
