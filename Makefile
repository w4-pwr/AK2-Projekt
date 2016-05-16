lcd: lcd.c
	gcc lcd.c -o lcd -lwiringPi -lwiringPiDev
run:
	./lcd
clean:
	rm -f lcd