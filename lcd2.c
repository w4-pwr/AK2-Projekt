#include <wiringPi.h>           //WiringPi headers
#include <stdlib.h>
#include <stdio.h>              //Needed for the printf function below
 
//Pin numbers below are the WiringPi pin numbers
 
#define LCD_RS  3               //Register select pin
#define LCD_E   0               //Enable Pin
#define LCD_D4  6               //Data pin 4
#define LCD_D5  1               //Data pin 5
#define LCD_D6  5               //Data pin 6
#define LCD_D7  4               //Data pin 7
 
 
 
 #define	LCD_CLEAR	0x01
#define	LCD_HOME	0x02
#define	LCD_ENTRY	0x04
#define	LCD_CTRL	0x08
#define	LCD_CDSHIFT	0x10
#define	LCD_FUNC	0x20
#define	LCD_CGRAM	0x40
#define	LCD_DGRAM	0x80

// Bits in the entry register

#define	LCD_ENTRY_SH		0x01
#define	LCD_ENTRY_ID		0x02

// Bits in the control register

#define	LCD_BLINK_CTRL		0x01
#define	LCD_CURSOR_CTRL		0x02
#define	LCD_DISPLAY_CTRL	0x04

// Bits in the function register

#define	LCD_FUNC_F	0x04
#define	LCD_FUNC_N	0x08
#define	LCD_FUNC_DL	0x10

#define	LCD_CDSHIFT_RL	0x04

#define MAX_LCDS 8

struct lcdDataStruct *lcds [MAX_LCDS];

 
 struct lcdDataStruct
{
  int bits, rows, cols ;
  int rsPin, strbPin ;
  int dataPins [8] ;
  int cx, cy ;
} ;


/*
 * strobe:
 *	Toggle the strobe (Really the "E") pin to the device.
 *	According to the docs, data is latched on the falling edge.
 *********************************************************************************
 */

static void strobe (const struct lcdDataStruct *lcd)
{

// Note timing changes for new version of delayMicroseconds ()

  digitalWrite (lcd->strbPin, 1) ; delayMicroseconds (50) ;
  digitalWrite (lcd->strbPin, 0) ; delayMicroseconds (50) ;
}

 
 static void sendDataCmd (const struct lcdDataStruct *lcd, unsigned char data)
{
  register unsigned char myData = data ;
  unsigned char          i, d4 ;

  if (lcd->bits == 4)
  {
    d4 = (myData >> 4) & 0x0F;
    for (i = 0 ; i < 4 ; ++i)
    {
      digitalWrite (lcd->dataPins [i], (d4 & 1)) ;
      d4 >>= 1 ;
    }
    strobe (lcd) ;

    d4 = myData & 0x0F ;
    for (i = 0 ; i < 4 ; ++i)
    {
      digitalWrite (lcd->dataPins [i], (d4 & 1)) ;
      d4 >>= 1 ;
    }
  }
  else
  {
    for (i = 0 ; i < 8 ; ++i)
    {
      digitalWrite (lcd->dataPins [i], (myData & 1)) ;
      myData >>= 1 ;
    }
  }
  strobe (lcd) ;
}



 static void putCommand (const struct lcdDataStruct *lcd, unsigned char command)
{
  digitalWrite (lcd->rsPin,   0) ;
  sendDataCmd  (lcd, command) ;
  delay (2) ;
}

static void put4Command (const struct lcdDataStruct *lcd, unsigned char command)
{
  register unsigned char myCommand = command ;
  register unsigned char i ;

  digitalWrite (lcd->rsPin,   0) ;

  for (i = 0 ; i < 4 ; ++i)
  {
    digitalWrite (lcd->dataPins [i], (myCommand & 1)) ;
    myCommand >>= 1 ;
  }
  strobe (lcd) ;
}


 
 int lcdInit ()
{
  static int initialised = 0 ;
	const int bits = 4;
  unsigned char func ;
  int i ;
  int lcdFd = -1 ;
  struct lcdDataStruct *lcd ;

  if (initialised == 0)
  {
    initialised = 1 ;
    for (i = 0 ; i < MAX_LCDS ; ++i)
      lcds [i] = NULL ;
  }


// Create a new LCD:

  for (i = 0 ; i < MAX_LCDS ; ++i)
  {
    if (lcds [i] == NULL)
    {
      lcdFd = i ;
      break ;
    }
  }

  if (lcdFd == -1)
    return -1 ;

  lcd = (struct lcdDataStruct *)malloc (sizeof (struct lcdDataStruct)) ;
  if (lcd == NULL)
    return -1 ;

  lcd->rsPin   = LCD_RS ;
  lcd->strbPin = LCD_E ;
  lcd->bits    = 4 ;		// For now - we'll set it properly later.
  lcd->rows    = 2 ;
  lcd->cols    = 16 ;
  lcd->cx      = 0 ;
  lcd->cy      = 0 ;

  lcd->dataPins [0] = LCD_D4 ;
  lcd->dataPins [1] = LCD_D5;
  lcd->dataPins [2] = LCD_D6;
  lcd->dataPins [3] = LCD_D7;
  lcd->dataPins [4] = 0 ;
  lcd->dataPins [5] = 0 ;
  lcd->dataPins [6] = 0 ;
  lcd->dataPins [7] = 0 ;

  lcds [lcdFd] = lcd ;

  digitalWrite (lcd->rsPin,   0) ; pinMode (lcd->rsPin,   OUTPUT) ;
  digitalWrite (lcd->strbPin, 0) ; pinMode (lcd->strbPin, OUTPUT) ;

  for (i = 0 ; i < bits ; ++i)
  {
    digitalWrite (lcd->dataPins [i], 0) ;
    pinMode      (lcd->dataPins [i], OUTPUT) ;
  }
  delay (35) ; // mS


// 4-bit mode?
//	OK. This is a PIG and it's not at all obvious from the documentation I had,
//	so I guess some others have worked through either with better documentation
//	or more trial and error... Anyway here goes:
//
//	It seems that the controller needs to see the FUNC command at least 3 times
//	consecutively - in 8-bit mode. If you're only using 8-bit mode, then it appears
//	that you can get away with one func-set, however I'd not rely on it...
//
//	So to set 4-bit mode, you need to send the commands one nibble at a time,
//	the same three times, but send the command to set it into 8-bit mode those
//	three times, then send a final 4th command to set it into 4-bit mode, and only
//	then can you flip the switch for the rest of the library to work in 4-bit
//	mode which sends the commands as 2 x 4-bit values.

  if (bits == 4)
  {
    func = LCD_FUNC | LCD_FUNC_DL ;			// Set 8-bit mode 3 times
    put4Command (lcd, func >> 4) ; delay (35) ;
    put4Command (lcd, func >> 4) ; delay (35) ;
    put4Command (lcd, func >> 4) ; delay (35) ;
    func = LCD_FUNC ;					// 4th set: 4-bit mode
    put4Command (lcd, func >> 4) ; delay (35) ;
    lcd->bits = 4 ;
  }
  else
  {
    func = LCD_FUNC | LCD_FUNC_DL ;
    putCommand  (lcd, func     ) ; delay (35) ;
    putCommand  (lcd, func     ) ; delay (35) ;
    putCommand  (lcd, func     ) ; delay (35) ;
  }

  if (lcd->rows > 1)
  {
    func |= LCD_FUNC_N ;
    putCommand (lcd, func) ; delay (35) ;
  }

// Rest of the initialisation sequence

  lcdDisplay     (lcdFd, TRUE) ;
  lcdCursor      (lcdFd, FALSE) ;
  lcdCursorBlink (lcdFd, FALSE) ;
  lcdClear       (lcdFd) ;

  putCommand (lcd, LCD_ENTRY   | LCD_ENTRY_ID) ;
  putCommand (lcd, LCD_CDSHIFT | LCD_CDSHIFT_RL) ;

  return lcdFd ;
}

//------------------------------------------------------------------------------

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


//inicjalizacja GPS
void initGPS(){
}

//inicjalizacja wyswietlacza
void initLCD(){
}

//wyswietlenie napisu oczekiwania
void displayWaiting(){
	//lcdPuts(lcd, "Waiting for signal...");
}

//wyswietlanie wspolrzednych na wyswieltaczu
void displayCoords(float lat, float lng){
	//lcdPuts(lcd, "Lat: ");
	//lcdPuts(lcd, "Lng: ");
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
    int lcd;                //Handle for LCD
    wiringPiSetup();        //Initialise WiringPi
 
    //Initialise LCD(int rows, int cols, int bits, int rs, int enable, int d0, int d1, int d2, int d3, int d4, int d5, int d6, int d7)
    if (lcd = lcdInit ()){
            printf ("lcdInit failed! \n");
            return -1 ;
    }
 
    lcdPosition(lcd,0,0);           //Position cursor on the first line in the first column
    lcdPuts(lcd, "Character LCD");  //Print the text on the LCD at the current cursor postion
    getchar();                      //Wait for key press
    lcdClear(lcd);                  //Clear the display



	initLCD();

	//czekanie na sygnal GPS
	int waiting = 1;
	while(waiting){
		displayWaiting();
		//sprawdzamy czy juz mamy polaczenie
		waiting = waitingForSingal(); 
	}

	saveCoords(0, 0);

	//sledzenie pozycji
	int tracking = 1;
	while(tracking){
		float lat = getLatitude();
		float lng = getLongitude();
		displayCoords(lat, lng);	
	}



}