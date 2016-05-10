#include <wiringPi.h>           //WiringPi headers
#include <lcd.h>                //LCD headers from WiringPi
#include <stdio.h>              //Needed for the printf function below
#include <unistd.h>
#include <string.h>
#include <iostream>
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


//oczekiwanie na sygnal
//1 - czeka
//0 - znalazl sygnal
int waitingForSingal(){

return 1; 

} 


//pobranie szerokosci z GPS
float getLatitude(){
return 51.4234;
}

//pobranie dlugosci z GPS
float getLongitude(){
return 17.4333;
}

void testGPS(){
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
	while(1)
	{
		int i=0;
		int c;
		if(c=serialGetchar(fd)==13||10)
		{
			for(i=0; i<6; i++)
			{
				if(serialGetchar(fd) == arr[i])
					flag++;
			}
		}
		if(flag==6)
		{
			flag=0;
			for(i=0; i<=65; i++)
				gps[i] = serialGetchar(fd);
		}
		printf("%d\n",gps);

	}
}

void testPars(){
FILE *file = fopen("gg.txt", "r");
char Buffer[1024] = {0};

while(fgets(Buffer,1024,file) != NULL)
{
	if(Buffer[0] == '$')
	{
		if (memcmp(Buffer+1,"GPGGA",5) == 0)
		{
		
			char *token = strtok(Buffer+6,",");
			int i = 0;
			while(token != NULL)
			{
				switch(i)
				{
				case 0:
			//		cout <<"Universal Time Coordinated (UTC): " <<token << endl;
					break;
				case 1:
					printf("Lat: %f\n", token);
					break;
				case 2:
			//		cout << "North or South :" <<token << endl;
					break;
				case 3:
					printf("Lng: %f\n", token);
					break;		
				case 4:
			//		cout <<"East or West :" <<token << endl;
					break;
				case 5:
			//		cout << "GPS Quality Indicator :"<<token << endl;
					break;
				case 6:
			//		cout << "Number of satellites in view, 00 - 12 :"<<token << endl;
					break;
				case 7:
			//		cout << "Horizontal Dilution of precision :"<<token << endl;
					break;
				case 8:
					//printf("Lat: %d\n", token);
					break;
				case 9:
			//		cout << "Units of antenna altitude, meters :"<<token << endl;
					break;
				case 10:
			//		cout << "Geoidal separation :"<<token << endl;
					break;
				case 11:
			//		cout << "Units of geoidal separation :"<<token << endl;
					break;
				case 12:
			//		cout << "Age of differential GPS data :"<<token << endl;
					break;
				case 13:
			//		cout << "Differential reference station ID :"<<token << endl;
					break;
				case 14:
			//		cout << "Checksum :"<<token << endl;
					break;
				default:
					break;
				}
				i++;
				token = strtok(NULL,",");
			}
		}
	}
}
}

//inicjalizacja GPS
int initGPS(){
	return 0;

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

//wyswietlenie napisu oczekiwania
void displayWaiting(int lcd){
	lcdClear(lcd); 
	lcdPosition(lcd,0,0);
	lcdPuts(lcd, "Czekanie na sygnal");   
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
 	int lcd = initLCD();
 	int gps = initGPS();
 	//testGPS();
 	testPars();
 	int hasSignal = waitingForSingal();
 	while(hasSignal){
 		sleep(1);
 		float lat = getLatitude();
 		float lng = getLongitude();
 		displayCoords(lcd, lat, lng);
 		saveCoords(lat, lng);
 	}
  
    getchar();
}
