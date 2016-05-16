#include <wiringPi.h>           //WiringPi headers
#include <lcd.h>                //LCD headers from WiringPi
#include <stdio.h>              //Needed for the printf function below
#include <unistd.h>
#include <string.h>
#include <wiringSerial.h>
//gcc lcd.c -o lcd -lwiringPi -lwiringPiDev


#define LCD_RS  3               //Register select pin
#define LCD_E   0               //Enable Pin
#define LCD_D4  6               //Data pin 4
#define LCD_D5  1               //Data pin 5
#define LCD_D6  5               //Data pin 6
#define LCD_D7  4               //Data pin 7


// ------------------------ GPS


//RPI-------------->GPS
//RX--------------->Tx
//TX--------------->Rx
//GND-------------->Gnd


 int lcd;

void parseNmea(char Buffer[255]){
	// printf("%s\n", Buffer);

if(Buffer[0] == '$')
	{
		if (memcmp(Buffer+1,"GPGLL",5) == 0)
		{
			char *token = strtok(Buffer+6,",");
			char *lat;
			int i = 0;
			while(token != NULL)
			{
				//lcdClear(lcd); 
				switch(i)
				{
				case 0:
					printf(" Lat: %s ", token);
					lcdPosition(lcd,0,0); 
					lcdPuts(lcd, "Lat: ");

					lcdPuts(lcd, token);
					break;
				case 1:
					printf("(%s)\n", token);
					// lcdPuts(lcd, token);
					break;
				case 2:
					printf(" Lng: %s ", token);
					lcdPosition(lcd,0,1); 
					lcdPuts(lcd, "Lng: "); 
					lcdPuts(lcd, token);
					break;		
				case 3:
				printf("(%s)\n\n", token);
				// lcdPuts(lcd, token);
					break;
				default:
					printf("----------- BLAD!\n");
					break;
				}
				i++;
				token = strtok(NULL,",");
			}
			printf("\n");
		}
	}

}


void startTracking(){
	char gps[65];
	int fd,flag=0; //uchwyt dla UART
	char arr[]="$GPGGA";

	if((fd = serialOpen("/dev/ttyAMA0",9600)) < 0)
	{
		printf("%s\n", "Nie udalo sie podlaczyc do UART");
		return;
	} else {
		printf("Podlaczono UART\n");
	}
	int i = 0;
	char buffer[255];
	while(1)
	{
		int c;
		if(c=serialGetchar(fd))
		{
			if(c != '\n'){
				buffer[i] = c;
			} else {
				buffer[i] = '\0';
				parseNmea(buffer);
				i = -1;
			}
			i++;
		}
	}
}

// ------------------------ LCD 
//inicjalizacja wyswietlacza
int initLCD(){
	int lcd;
	if (lcd = lcdInit (2, 16,4, LCD_RS, LCD_E ,LCD_D4 , LCD_D5, LCD_D6,LCD_D7,0,0,0,0)){
        printf ("Nie udana inicjalizacja LCD! \n");
            return -1;
    }
    return lcd;
}

//wyswietlanie wspolrzednych na wyswieltaczu
void displayCoords(int lcd, float lat, float lng){
	lcdClear(lcd); 
	lcdPuts(lcd, "Lat: ");
	lcdPosition(lcd,0,1);   
	lcdPuts(lcd, "Lng: ");
}

//zapis wspolrzednych do pliku csv
void saveCoords(float lat, float lng){
	//save to file
	FILE *f = fopen("coords.csv", "w");
	if (f == NULL)
	{
		printf("Nie mozna otworzyc pliku!\n");
	}
	fprintf(f, "%f,%f\n", lat, lng);
	fclose(f);
}

//-------------------------------------------------------------------------------

int main()
{
 	//inicjalizacja 
 	wiringPiSetup();
	lcd = initLCD();
 	startTracking();
	getchar();
}
