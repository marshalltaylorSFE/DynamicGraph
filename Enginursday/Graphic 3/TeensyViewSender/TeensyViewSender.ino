//**********************************************************************//
//  BEERWARE LICENSE
//
//  This code is free for any use provided that if you meet the author
//  in person, you buy them a beer.
//
//  This license block is BeerWare itself.
//
//  Written by:  Marshall Taylor
//  Created:  March 14, 2016
//
//**********************************************************************//

//**UART #defines*****************************//
//NOT PRESENT ON PROTO-2
//#define DEBUGSERIALPORT Serial 
//#define REMOTELINKPORT Serial1 //link port

#define REMOTELINKPORT Serial //For now, dump packets on-screen

//#include "HOS_char.h"
#include <math.h>
#include "uCPacketClass.h"
#include "userPacketDefs.h"



#include "TeensyView.h"  // Include the TeensyView library
#include "PanelComponents.h"
#include "PanelComponents_Debug.h"
#include "HardwareInterfaces.h"

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveformSine   sine1;          //xy=644,1501
AudioSynthWaveformSine   sine2;          //xy=644,1541
AudioAnalyzePeak         peak1;          //xy=866,1486
AudioOutputAnalog        dac1;           //xy=868,1558
AudioOutputI2S           i2s2;           //xy=881,1522
AudioConnection          patchCord1(sine1, 0, i2s2, 0);
AudioConnection          patchCord2(sine1, peak1);
AudioConnection          patchCord3(sine2, 0, i2s2, 1);
AudioConnection          patchCord4(sine2, dac1);
AudioControlSGTL5000     sgtl5000_1;     //xy=831,1627
// GUItool: end automatically generated code



//////////////////////////////////
// TeensyView Object Declaration //
//////////////////////////////////
#define PIN_RESET 2 
#define PIN_DC    3 
#define PIN_CS    4 
#define PIN_SCK   14
#define PIN_MOSI  7 

TeensyView oled(PIN_RESET, PIN_DC, PIN_CS, PIN_SCK, PIN_MOSI);

//Set knob names to analog input names here (use for remapping)
// Knobs go:
//  1   2   3
//  (OLED)
//  4   5

//Globals
uint32_t maxTimer = 60000000;
uint32_t maxInterval = 2000000;

uint32_t packetNumber = 0;


#define	KNOB1POS A1
#define	KNOB2POS A2
#define	KNOB3POS A3
#define	KNOB4POS A6
#define	KNOB5POS A7

Windowed10BitKnob_Debug knob1;
Windowed10BitKnob_Debug knob2;
Windowed10BitKnob_Debug knob3;
Windowed10BitKnob_Debug knob4;
Simple10BitKnob_Debug knob5;

uint16_t debugCounter = 0;

uint8_t knob4OnlyMode = 0;
uint8_t requestClearScreenFlag = 0;
uint8_t screenClearedFlag = 0;
uint8_t requestKnob4ScreenFlag = 0;
uint8_t knob4ScreenFlag = 0;

//Used to prevent free run looping
uint32_t next;


#define LEDPIN 13
#include "timerModule32.h"
#include "stdint.h"

IntervalTimer myTimer; //Interrupt for Teensy

//**32 bit timer classes *********************//  
TimerClass32 debugTimer( 1000000 ); //1 seconds
TimerClass32 serialSendTimer( 30000 ); //0.015 seconds
TimerClass32 remoteInputTimer( 3000 );

//--tick variable for interrupt driven timer1
elapsedMicros usTickInput = 0;
uint32_t usTicks = 0;
uint8_t usTicksLocked = 1; //start locked out

//**Packets***********************************//
knobPacket packetToHost;
knobPacket packetFromHost;

//**Serial Machine****************************//
uCPacketUART dataLinkHandler((HardwareSerial*)&REMOTELINKPORT, 64); //64 byte buffers


//variables to send data back to processing
uint16_t triggered = 0;
uint16_t lastTriggeredData = 0;

void setup()
{
  REMOTELINKPORT.begin(250000);
  
  //dataLinkHandler.initialize();

  packetToHost.packetNumber = 0;
  
  pinMode( LEDPIN, OUTPUT );

	//**** Knob Setup ****//
 	knob1.setHardware(new ArduinoAnalogIn( KNOB1POS ));
	knob2.setHardware(new ArduinoAnalogIn( KNOB2POS ));
	knob3.setHardware(new ArduinoAnalogIn( KNOB3POS ));
	knob4.setHardware(new ArduinoAnalogIn( KNOB4POS ));
	knob5.setHardware(new ArduinoAnalogIn( KNOB5POS ));

	//set params
	knob1.setLowerFloatVal(0);
	knob1.setUpperFloatVal(1.0);
	knob1.setLowerUIntVal(0);
	knob1.setUpperUIntVal(11);
	knob1.setWindow(10);
	knob1.setSamplesAveraged(10);
	
	knob2.setLowerUIntVal(1);
	knob2.setUpperUIntVal(128);
	knob2.setWindow(10);
	knob2.setSamplesAveraged(10);
	
	knob3.setLowerUIntVal(1);
	knob3.setUpperUIntVal(15);
	knob3.setWindow(10);
	knob3.setSamplesAveraged(10);
	
	knob4.setLowerUIntVal(0);
	knob4.setUpperUIntVal(1023);
	knob4.setLowerFloatVal(0);
	knob4.setUpperFloatVal(440);
	knob4.setWindow(10);
	knob4.setSamplesAveraged(10);
	
	//knob5.setLowerUIntVal(0);
	//knob5.setUpperUIntVal(1023);
	//knob5.setLowerFloatVal(0);
	//knob5.setUpperFloatVal(440);
	//knob5.setWindow(10);
	//knob5.setSamplesAveraged(10);
//                                        	//Run update a few times to fill the averaging buffers
//                                        	for(int i = 0; i < 10; i++) 
//                                        	{
//                                        		knob1.update();
//                                        		knob2.update();
//                                        		knob3.update();
//                                        		knob4.update();
//                                        		knob5.update();
//                                        	}

	//**** Timer Setup ****//
	next = millis() + 2000;

	//**** Audio Setup ****//
	AudioNoInterrupts();

	AudioMemory(35);

	sgtl5000_1.enable();
	sgtl5000_1.volume(0.8); //Force check first volume
	sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
	sgtl5000_1.unmuteHeadphone();
	sgtl5000_1.lineInLevel(1); //use 2 for 1:1
	sgtl5000_1.lineOutLevel(13);

	//Configure initial values here
	//sine1.frequency(440);
	//sine2.frequency(1760);

	sine1.amplitude(0.0);
	sine2.amplitude(0.0);
	
	AudioInterrupts();

	//Initialize the OLED
	oled.begin();
	oled.command(COMSCANINC); //Vertical flip
	oled.command(0xA0); //Horizontal flip
	// clear(ALL) will clear out the OLED's graphic memory.
	// clear(PAGE) will clear the Arduino's display buffer.
	oled.clear(ALL);  // Clear the display's memory (gets rid of artifacts)
	// To actually draw anything on the display, you must call the
	// display() function. 
	oled.display(); 
	// Give the splash screen some time to shine
	delay(2000);
	oled.clear(PAGE);
	oled.display(); 


}

void loop()
{
	//Update the timers, but only once per interrupt
	//if( usTicksLocked == 0 )
	{
		//**Give the timers the current time**********//  
		serialSendTimer.update(usTicks);
		remoteInputTimer.update(usTicks);
		debugTimer.update(usTicks);
		
		//Done?  Lock it back up
		//I'm still not sure if this does anything
		//usTicksLocked = 1;
	}

	//**Read the input packet*********************//  
	if(remoteInputTimer.flagStatus() == PENDING)
	{
		dataLinkHandler.burstReadInputBuffer();
		if( dataLinkHandler.available() == sizeof packetFromHost )
		{
			dataLinkHandler.getPacket( (uint8_t *)&packetFromHost, sizeof packetFromHost );
			//Now do operations on returned packet
			//if( packetFromHost.someVar == blargle ) ...
		}
	}

	if(serialSendTimer.flagStatus() == PENDING)
	{
	//Check for new data
		uint8_t tempStatus = 0;
		tempStatus = 1; //This forces packet send every time.  Use to block tx when necessary.

		//Read knobs -- a 10 bit number
		//Get the knob values
		knob1.freshen(0);
		knob2.freshen(0);
		knob3.freshen(0);
		knob4.freshen(0);
		knob5.freshen(0);
		
		//Set audio platform parameters based on those values
		uint8_t displayData = 0;
		if(knob1.serviceChanged())
		{
			if( knob4OnlyMode == 0 )
			{
				oled.setCursor(0,0);
				oled.print("             ");
				oled.setCursor(0,0);
				oled.print("Volume: ");
				oled.print((uint8_t)knob1.getAsUInt16());
				oled.display();
			}
			//Take action
			sine1.amplitude(knob1.getAsFloat());
			sine2.amplitude(knob1.getAsFloat());
			if(knob1.getAsUInt16() == 0)
			{
				knob4OnlyMode = 1;
				requestClearScreenFlag = 0;
				screenClearedFlag = 0;
				requestKnob4ScreenFlag = 1;
			}
			else
			{
				knob4OnlyMode = 0;
				requestClearScreenFlag = 1;
				requestKnob4ScreenFlag = 0;
				knob4ScreenFlag = 0;
			}
			displayData = 1;
		}
		if(knob2.serviceChanged())
		{
			if( knob4OnlyMode == 0 )
			{
				oled.setCursor(0,0);
				oled.print("             ");
				oled.setCursor(0,0);
				oled.print("Hyst.: ");
				oled.print(knob2.getAsUInt16() - 1);
				oled.display();
			}
			displayData = 2;
			knob4.setWindow(knob2.getAsUInt16());
			//knob5.setWindow(knob2.getAsUInt16());
		}
		if(knob3.serviceChanged())
		{
			if( knob4OnlyMode == 0 )
			{
				oled.setCursor(0,0);
				oled.print("             ");
				oled.setCursor(0,0);
				oled.print("Ave'd: ");
				oled.print(knob3.getAsUInt16() - 1);
				oled.display();
			}
			displayData = 3;
			knob4.setSamplesAveraged(knob3.getAsUInt16());
			//knob5.setSamplesAveraged(knob3.getAsUInt16());
		}
		if((knob4.serviceChanged())||((requestKnob4ScreenFlag == 1)&&(knob4ScreenFlag == 0)))
		{
			knob4ScreenFlag = 1;
			requestKnob4ScreenFlag = 0;
			
			if( knob4OnlyMode == 0 )
			{
				oled.setCursor(0,0);
				oled.print("             ");
				oled.setCursor(0,0);
				oled.print("L Hz: ");
				oled.print(knob4.getAsFloat() + 30);
				oled.display();
			}
			else
			{
				oled.setCursor(0,0);
				oled.print("                 ");
				oled.setCursor(0,0);
				oled.print("Knob 4 Only: ");
				oled.print(knob4.getAsUInt16());
				oled.setFontType(2);  // Set font to type 1
				oled.setCursor(4,10);
				uint16_t tempKnobValue = knob4.getAsUInt16();
				for( int i = 9; i >= 0; i--)
				{
					oled.print((tempKnobValue >> i) & 0x0001);
				}
				oled.display();
				oled.setFontType(0);  // Set font to type 0
			}
			displayData = 4;
			sine1.frequency(knob4.getAsFloat() + 30);
		}
		if(knob5.serviceChanged())
		{
			if( knob4OnlyMode == 0 )
			{
				oled.setCursor(0,0);
				oled.print("             ");
				oled.setCursor(0,0);
				oled.print("R Hz: ");
				oled.print(knob5.getState());
				oled.display();
			}
			displayData = 5;
			//sine2.frequency(knob5.getAsFloat() + 30.25);
			triggered = 1;
			lastTriggeredData = knob5.getLastState();
			
		}
		
		if((requestClearScreenFlag == 1)&&(screenClearedFlag == 0))
		{
			screenClearedFlag = 1;
			requestClearScreenFlag = 0;
			oled.clear(PAGE);
			oled.display();
		}

		packetToHost.rawADC = knob5.getState();
		packetToHost.filteredADC = lastTriggeredData;
		//packetToHost.var1 = knob5.getWindowUpper();
		//packetToHost.var2 = knob5.getWindowLower();
		packetToHost.event = triggered;
		if(triggered == 1) triggered = 0;
		packetToHost.packetNumber++;
		// If new, ship it!
		if( tempStatus )
		{
			dataLinkHandler.sendPacket( (uint8_t *)&packetToHost, sizeof packetToHost );
		}
		
		
		
	}
	//**Copy to make a new timer******************//  
	//if(usTimerA.flagStatus() == PENDING)
	//{
	//	//User code
	//}

	if(debugTimer.flagStatus() == PENDING)
	{
	}

	//Do the ISR with the teensy built-in timer
	if(usTickInput != usTicks)
	{
		uint32_t returnVar = 0;
		if(usTickInput >= ( maxTimer + maxInterval ))
		{
		usTickInput = usTickInput - maxTimer;
		
		}
		usTicks = usTickInput;
		//usTicksMutex = 0;  //unlock
	}
}
