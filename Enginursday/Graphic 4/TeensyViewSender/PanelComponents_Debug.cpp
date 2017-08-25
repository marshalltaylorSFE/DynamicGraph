#include "stdint.h"
#include "PanelComponents.h"
#include "PanelComponents_Debug.h"
#include "HardwareInterfaces.h"
#include "Arduino.h"
#include "CircularBuffer.h"

//---Simple10BitKnob_Debug--------------------------------------------------------
void Simple10BitKnob_Debug::freshen( uint16_t msTickDelta )
{
	//Throw away input
	hardwareInterface->readHardware();
	//Collect the data
	KnobDataObject tempObject;
//	KnobDataObject * tempObject = new KnobDataObject;
//		Serial.println("knobFreshen");
//		Serial.print("Temp ");
//		dumpObject(&tempObject);
//Serial.print((uint32_t)&(*tempObject), HEX);
//Serial.println("Created");
	hardwareInterface->getData(&tempObject);
	
	state = *(uint16_t *)tempObject.data;
	
	if(( state > lastState + hysteresis )||( state < lastState - hysteresis ))
	{
		lastState = state;
		
		newData = 1;
	}
//Serial.print("Temp ");
//dumpObject(tempObject);
//Serial.print((uint32_t)&(*tempObject), HEX);
//Serial.println("Destroyed");
//	delete tempObject;

}

void Simple10BitKnob_Debug::setHysteresis( uint8_t input )
{
	hysteresis = input;
}

uint16_t Simple10BitKnob_Debug::getState( void )
{
  return state;
}

uint16_t Simple10BitKnob_Debug::getLastState( void )
{
  return lastState;
}

//---Complex10BitKnob_Debug--------------------------------------------
void Complex10BitKnob_Debug::freshen( uint16_t msTickDelta )
{
	//Throw away input
	//Cause the interface to get the data
	hardwareInterface->readHardware();
	//Collect the data
	KnobDataObject tempObject;
	hardwareInterface->getData(&tempObject);
	
//		Serial.println("knobFreshen");
//		Serial.print("Temp ");
//		dumpObject(&tempObject);

	uint16_t tempState = *(uint16_t *)tempObject.data;
	//Read the knob value into the buffer
	//Serial.println(tempState);
	lastRaw = tempState;

	currentValue = tempState;
}

uint16_t Complex10BitKnob_Debug::getState( void )
{
  return (uint16_t)currentValue;
}

uint8_t Complex10BitKnob_Debug::getAsUInt8( void )
{
	newData = 0;
	return currentValue >> 2;
}
int16_t Complex10BitKnob_Debug::getAsInt16( void )
{
	uint32_t tempValue = currentValue;
	if( tempValue < lowerKnobVal ) tempValue = lowerKnobVal;
	if( tempValue > upperKnobVal ) tempValue = upperKnobVal;
	// now find scalar from 0 to 1
	float inputPercent = ((float)(tempValue - lowerKnobVal))/(float)(upperKnobVal - lowerKnobVal);
	float outputRange = upperIntVal - lowerIntVal;
	newData = 0;
	return lowerIntVal + (inputPercent * outputRange);
}
uint16_t Complex10BitKnob_Debug::getAsUInt16( void )
{
	uint32_t tempValue = currentValue;
	if( tempValue < lowerKnobVal ) tempValue = lowerKnobVal;
	if( tempValue > upperKnobVal ) tempValue = upperKnobVal;
	// now find scalar from 0 to 1
	float inputPercent = ((float)(tempValue - lowerKnobVal))/(float)(upperKnobVal - lowerKnobVal);
	float outputRange = upperUIntVal - lowerUIntVal;
	newData = 0;
	return lowerUIntVal + (inputPercent * outputRange);
}
float Complex10BitKnob_Debug::getAsFloat( void )
{
	uint32_t tempValue = currentValue;
	if( tempValue < lowerKnobVal ) tempValue = lowerKnobVal;
	if( tempValue > upperKnobVal ) tempValue = upperKnobVal;
	// now find scalar from 0 to 1
	float inputPercent = ((float)(tempValue - lowerKnobVal))/(float)(upperKnobVal - lowerKnobVal);
	float outputRange = upperFloatVal - lowerFloatVal;
	newData = 0;
	return lowerFloatVal + (inputPercent * outputRange);
}

uint16_t Complex10BitKnob_Debug::getRaw( void )
{
  return (uint16_t)lastRaw;
}

void Complex10BitKnob_Debug::setLowerKnobVal( uint16_t input )
{
	if( input < 0 )input = 0;
	if( input > 1023 )input = 1023;
	lowerKnobVal = input;
}

void Complex10BitKnob_Debug::setUpperKnobVal( uint16_t input )
{
	if( input < 0 )input = 0;
	if( input > 1023 )input = 1023;
	upperKnobVal = input;
}

void Complex10BitKnob_Debug::setLowerFloatVal( float input )
{
	lowerFloatVal = input;
}

void Complex10BitKnob_Debug::setUpperFloatVal( float input )
{
	upperFloatVal = input;
}

void Complex10BitKnob_Debug::setLowerIntVal( int16_t input )
{
	lowerIntVal = input;
}

void Complex10BitKnob_Debug::setUpperIntVal( int16_t input )
{
	upperIntVal = input;
}

void Complex10BitKnob_Debug::setLowerUIntVal( uint16_t input )
{
	lowerUIntVal = input;
}

void Complex10BitKnob_Debug::setUpperUIntVal( uint16_t input )
{
	upperUIntVal = input;
}

//---Hysteresis10BitKnob_Debug--------------------------------------------------------
Hysteresis10BitKnob_Debug::Hysteresis10BitKnob_Debug( void ) : values(16), averages(16)
{
}
void Hysteresis10BitKnob_Debug::freshen( uint16_t msTickDelta )
{
	//Throw away input
	//Cause the interface to get the data
	hardwareInterface->readHardware();
	//Collect the data
	KnobDataObject tempObject;
	hardwareInterface->getData(&tempObject);
	
//		Serial.println("knobFreshen");
//		Serial.print("Temp ");
//		dumpObject(&tempObject);

	uint16_t tempState = *(uint16_t *)tempObject.data;
	//Read the knob value into the buffer
	//Serial.println(tempState);
	values.write(tempState);
	int32_t accumulator = 0;
	for( int i = 0; i < samplesAveraged; i++ )
	{
		accumulator += values.read(i);
	}
	averages.write(accumulator/samplesAveraged);
	if(hystState == 0)
	{
		if( averages.read(0) > currentValue )
		{
			//Allow current to go up only
			currentValue = averages.read(0);
			newData = 1;
		}
		if( averages.read(0) < currentValue - hysteresis )
		{
			//toggle behavior
			hystState = 1;
			currentValue = averages.read(0);
			newData = 1;			
		}
	}
	else
	{
		if( averages.read(0) < currentValue )
		{
			//Allow current to go up only
			currentValue = averages.read(0);
			newData = 1;
		}
		if( averages.read(0) > currentValue + hysteresis )
		{
			//toggle behavior
			hystState = 0;
			currentValue = averages.read(0);
			newData = 1;			
		}
	}
	

}

void Hysteresis10BitKnob_Debug::setHysteresis( uint8_t input )
{
	hysteresis = input;
}

void Hysteresis10BitKnob_Debug::setSamplesAveraged( uint8_t input )
{
	samplesAveraged = input;
}

//---Windowed10BitKnob_Debug--------------------------------------------------------
Windowed10BitKnob_Debug::Windowed10BitKnob_Debug( void ) : values(16), averages(16)
{
}

void Windowed10BitKnob_Debug::freshen( uint16_t msTickDelta )
{
	//Throw away input
	//Cause the interface to get the data
	hardwareInterface->readHardware();
	//Collect the data
	KnobDataObject tempObject;
	hardwareInterface->getData(&tempObject);
	
//		Serial.println("knobFreshen");
//		Serial.print("Temp ");
//		dumpObject(&tempObject);

	uint16_t tempState = *(uint16_t *)tempObject.data;
	lastRaw = tempState;

	//Read the knob value into the buffer
	//Serial.println(tempState);
	values.write(tempState);
	int32_t accumulator = 0;
	for( int i = 0; i < samplesAveraged; i++ )
	{
		accumulator += values.read(i);
	}
	averages.write(accumulator/samplesAveraged);
	if( averages.read(0) > windowUpper )
	{
		moveWindowUpper( averages.read(0) );
		newData = 1;
	}
	if( averages.read(0) < windowLower )
	{
		moveWindowLower( averages.read(0) );
		newData = 1;
	}
	uint32_t temp = windowLower + (window >> 1);
	//scale it
	currentValue = ((temp - (window >> 1)) << 10 ) / (1024 - window );
}

int16_t Windowed10BitKnob_Debug::getWindowUpper( void )
{
	return windowUpper;
}

int16_t Windowed10BitKnob_Debug::getWindowLower( void )
{
	return windowLower;
}

void Windowed10BitKnob_Debug::setSamplesAveraged( uint8_t input )
{
	samplesAveraged = input;
}

void Windowed10BitKnob_Debug::setWindow( int16_t input )
{
	window = input;
}

void Windowed10BitKnob_Debug::moveWindowUpper( uint16_t input )
{
	windowUpper = input;
	windowLower = input - window;
}

void Windowed10BitKnob_Debug::moveWindowLower( uint16_t input )
{
	windowLower = input;
	windowUpper = input + window;
	
}
