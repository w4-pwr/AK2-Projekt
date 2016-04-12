#include <wiringPi.h>           //WiringPi headers
#include <lcd.h>                //LCD headers from WiringPi
#include <stdio.h>              //Needed for the printf function below
 
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

return 0; 

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
		fprintf(stderr,"Nie udalo sie podlaczyc do UART: %s \n", strerror(errno));
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
		printf("%d",gps);

}

//inicjalizacja GPS
void initGPS(){


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
	lcdPosition(lcd,1,0);   
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

 	int hasSignal = waitingForSingal();
 	while(hasSignal){
 		float lat = getLatitude();
 		float lng = getLongitude();
 		displayCoords(lcd, lat, lng);
 		saveCoords(lat, lng);
 	}
  
    getchar();
}