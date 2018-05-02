//Handles communication with the Adafruit Bluefruit Android/iOS App
//Based on Adafruit Bluefruit controller.ino example

#include <string.h>
#include <Arduino.h>
#include <SPI.h>

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "Bluefruit.h"

// ----------------------------------------------------------------------------------------------
// HARDWARE UART SETTINGS
// ----------------------------------------------------------------------------------------------
#define BLUEFRUIT_HWSERIAL_NAME      Serial1
#define BLUEFRUIT_UART_MODE_PIN        6    // Set to -1 if unused
#define BLUEFRUIT_UART_CTS_PIN         11   // Required for software serial!
#define BLUEFRUIT_UART_RTS_PIN         -1   // Optional, set to -1 if unused

// ----------------------------------------------------------------------------------------------
// COMMON SETTINGS
// ----------------------------------------------------------------------------------------------
//#define BUFSIZE                        256   // Size of the read buffer for incoming data
#define FACTORYRESET_ENABLE				0
#define VERBOSE_MODE                   true  // If set to 'true' enables debug output
#define BLE_READPACKET_TIMEOUT         500   // Timeout in ms waiting to read a response

#define PACKET_ACC_LEN                  (15)
#define PACKET_GYRO_LEN                 (15)
#define PACKET_MAG_LEN                  (15)
#define PACKET_QUAT_LEN                 (19)
#define PACKET_BUTTON_LEN               (5)
#define PACKET_COLOR_LEN                (6)
#define PACKET_LOCATION_LEN             (15)

//    READ_BUFSIZE            Size of the read buffer for incoming packets
#define READ_BUFSIZE                    (20)

/* Buffer to hold incoming characters */
uint8_t packetbuffer[READ_BUFSIZE+1];

Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME);

// ----------------------------------------------------------------------------------------------
// App Data
// ----------------------------------------------------------------------------------------------
bool UpdateFlag = 0; // Shared Update Flag

bool Bluefruit_ButtonUp;
bool Bluefruit_ButtonDown;
bool Bluefruit_ButtonLeft;
bool Bluefruit_ButtonRight;
bool Bluefruit_Button1;
bool Bluefruit_Button2;
bool Bluefruit_Button3;
bool Bluefruit_Button4;
uint8_t ColorR;
uint8_t ColorG;
uint8_t ColorB;

//typedef struct
//{
//	uint8_t ButtonUp 	: 1;
//	uint8_t ButtonDown 	: 1;
//	uint8_t ButtonLeft	: 1;
//	uint8_t ButtonRight	: 1;
//	uint8_t Button1		: 1;
//	uint8_t Button2		: 1;
//	uint8_t Button3		: 1;
//	uint8_t Button4		: 1;
//} BUTTON_BITS_T;
//
//BUTTON_BITS_T ButtonStates;
//BUTTON_BITS_T ButtonChanged;


/**************************************************************************/
/*!
    @brief  Casts the four bytes at the specified address to a float
*/
/**************************************************************************/
float parsefloat(uint8_t *buffer)
{
  float f = ((float *)buffer)[0];
  return f;
}

/**************************************************************************/
/*!
    @brief  Prints a hexadecimal value in plain characters
    @param  data      Pointer to the byte data
    @param  numBytes  Data length in bytes
*/
/**************************************************************************/
void printHex(const uint8_t * data, const uint32_t numBytes)
{
  uint32_t szPos;
  for (szPos=0; szPos < numBytes; szPos++)
  {
    Serial.print(F("0x"));
    // Append leading 0 for small values
    if (data[szPos] <= 0xF)
    {
      Serial.print(F("0"));
      Serial.print(data[szPos] & 0xf, HEX);
    }
    else
    {
      Serial.print(data[szPos] & 0xff, HEX);
    }
    // Add a trailing space if appropriate
    if ((numBytes > 1) && (szPos != numBytes - 1))
    {
      Serial.print(F(" "));
    }
  }
  Serial.println();
}

/**************************************************************************/
/*!
    @brief  Waits for incoming data and parses it
*/
/**************************************************************************/
uint8_t buildPacket(Adafruit_BLE *ble, uint16_t timeout)
{
  static uint16_t replyidx = 0;

  bool packetComplete = 0;
  static bool doCheckSum;

  uint16_t length = 0;

  ///memset(packetbuffer, 0, READ_BUFSIZE);

  	 while (ble->available()) {
	  char c =  ble->read();
	  if (c == '!') {
		replyidx = 0;
	  }
	  packetbuffer[replyidx] = c;
	  replyidx++;
	  //timeout = origtimeout;
	  if (replyidx >= READ_BUFSIZE){
		  replyidx = 0;
	  }
	}

	  if (!replyidx)  // no data or timeout
		return 0;
	  if (packetbuffer[0] != '!')  // doesn't start with '!' packet beginning
		return 0;

    if ((packetbuffer[1] == 'A') && (replyidx == PACKET_ACC_LEN))
    	packetComplete = 1;
    if ((packetbuffer[1] == 'G') && (replyidx == PACKET_GYRO_LEN))
    	packetComplete = 1;
    if ((packetbuffer[1] == 'M') && (replyidx == PACKET_MAG_LEN))
    	packetComplete = 1;
    if ((packetbuffer[1] == 'Q') && (replyidx == PACKET_QUAT_LEN))
    	packetComplete = 1;
    if ((packetbuffer[1] == 'B') && (replyidx == PACKET_BUTTON_LEN))
    	packetComplete = 1;
    if ((packetbuffer[1] == 'C') && (replyidx == PACKET_COLOR_LEN))
    	packetComplete = 1;
    if ((packetbuffer[1] == 'L') && (replyidx == PACKET_LOCATION_LEN))
    	packetComplete = 1;

	  if(!packetComplete)
		return 0;

	  packetbuffer[replyidx] = 0;  // null term
	  length = replyidx;
	  replyidx = 0; // next call to this function will return 0 unless there is new data
	  return length;

//    if (doCheckSum)
//    {
//	  // check checksum!
//	  uint8_t xsum = 0;
//	  uint8_t checksum = packetbuffer[replyidx-1];
//
//	  for (uint8_t i=0; i<replyidx-1; i++) {
//		xsum += packetbuffer[i];
//	  }
//	  xsum = ~xsum;
//
//	  // Throw an error message if the checksum's don't match
//	  if (xsum != checksum)
//	  {
//		Serial.print("Checksum mismatch in packet : ");
//		printHex(packetbuffer, replyidx+1);
//		return 0;
//	  }
//
//	  // checksum passed!
//	  return replyidx;
//    }
//    else
//    	return 0;
}



// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void Bluefruit_Init(void)
{
  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE, 0) )
  {
    //error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
	  Serial.println(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  else
  {
	  Serial.println( F("OK!") );

	  if ( FACTORYRESET_ENABLE )
	  {
		/* Perform a factory reset to make sure everything is in a known state */
		Serial.println(F("Performing a factory reset: "));
		if ( ! ble.factoryReset() ){
		  error(F("Couldn't factory reset"));
		}
	  }

	  /* Disable command echo from Bluefruit */
	  ble.echo(false);

	  Serial.println("Requesting Bluefruit info:");
	  /* Print Bluefruit information */
	  ble.info();

	  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in Controller mode"));
	  Serial.println();

	  //ble.verbose(false);  // debug info is a little annoying after this point!

	  Serial.println(F("***********************"));

	  // Set Bluefruit to DATA mode
	  Serial.println( F("Switching to DATA mode!") );
	  ble.setMode(BLUEFRUIT_MODE_DATA);

	  Serial.println(F("***********************"));
  }
}

void Bluefruit_Restart(void)
{
	ble.begin(0, 0);
}

char Bluefruit_GetPacketType()
{
	return packetbuffer[1];
}

bool Bluefruit_GetUpdateFlag()		{return UpdateFlag;}
void Bluefruit_ResetUpdateFlag()	{UpdateFlag = 0;}

uint8_t Bluefruit_GetColorR() 		{return ColorR;}
uint8_t Bluefruit_GetColorG() 		{return ColorG;}
uint8_t Bluefruit_GetColorB() 		{return ColorB;}
bool Bluefruit_GetButton1()			{return Bluefruit_Button1;}
bool Bluefruit_GetButton2()			{return Bluefruit_Button2;}
bool Bluefruit_GetButton3()			{return Bluefruit_Button3;}
bool Bluefruit_GetButton4()			{return Bluefruit_Button4;}
bool Bluefruit_GetButtonUp()		{return Bluefruit_ButtonUp;}
bool Bluefruit_GetButtonDown()		{return Bluefruit_ButtonDown;}
bool Bluefruit_GetButtonLeft()		{return Bluefruit_ButtonLeft;}
bool Bluefruit_GetButtonRight()		{return Bluefruit_ButtonRight;}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void Bluefruit_Poll()
{
  /* Wait for new data to arrive */
  uint8_t len = buildPacket(&ble, BLE_READPACKET_TIMEOUT);
  if (len == 0) return;

  /* Got a packet! */
  // printHex(packetbuffer, len);
  UpdateFlag = 1;

  // Color
  if (packetbuffer[1] == 'C') {
	ColorR = packetbuffer[2];
	ColorG = packetbuffer[3];
	ColorB = packetbuffer[4];
//		Serial.print ("RGB #");
//		if (ColorR < 0x10) Serial.print("0");
//		Serial.print(ColorR, HEX);
//		if (ColorG < 0x10) Serial.print("0");
//		Serial.print(ColorG, HEX);
//		if (ColorB < 0x10) Serial.print("0");
//		Serial.println(ColorB, HEX);
  }

  // Buttons
  if (packetbuffer[1] == 'B') {
    uint8_t buttnum = packetbuffer[2] - '0';
    boolean pressed = packetbuffer[3] - '0';
    switch (buttnum)
    {
    	case 1:
    		Bluefruit_Button1 = pressed;
    		break;
    	case 2:
    		Bluefruit_Button2 = pressed;
    		break;
    	case 3:
    		Bluefruit_Button3 = pressed;
    		break;
    	case 4:
    		Bluefruit_Button4 = pressed;
    		break;
    	case 5:
    		Bluefruit_ButtonUp = pressed;
    		break;
    	case 6:
    		Bluefruit_ButtonDown = pressed;
    		break;
    	case 7:
    		Bluefruit_ButtonLeft = pressed;
    		break;
    	case 8:
    		Bluefruit_ButtonRight = pressed;
    		break;
    	default:
    		break;
    }
//    Serial.print ("Button "); Serial.print(buttnum);
//    if (pressed) {
//      Serial.println(" pressed");
//    } else {
//      Serial.println(" released");
//    }
  }

  //memset(packetbuffer, 0, READ_BUFSIZE);
}

