#ifndef PANEL_COMPONENTS_DEBUG_H
#define PANEL_COMPONENTS_DEBUG_H

#include "stdint.h"
#include "HardwareInterfaces.h"
#include "timeKeeper.h"
#include "CircularBuffer.h"
#include "PanelComponents.h"

//---Simple10BitKnob---------------------------------------------
class Simple10BitKnob_Debug : public KnobParentClass
{
public:
	Simple10BitKnob_Debug( void ){};
	virtual void freshen( uint16_t msTickDelta );
	void setHysteresis( uint8_t );
	uint16_t getState( void );
	uint16_t getThreshold( void );
protected:
	uint16_t state;
	uint16_t lastState;
	uint8_t hysteresis = 2;
	int8_t lastSlope = 1;
	uint16_t threshold = 0;

};

//---Complex10BitKnob_Debug--------------------------------------------
class Complex10BitKnob_Debug : public KnobParentClass
{
public:
	Complex10BitKnob_Debug( void ){};
	virtual void freshen( uint16_t msTickDelta );
	uint16_t getState( void );
	uint8_t getAsUInt8( void );
	int16_t getAsInt16( void );
	uint16_t getAsUInt16( void );
	float getAsFloat( void );
	uint16_t getRaw( void );
	void setLowerKnobVal( uint16_t input );
	void setUpperKnobVal( uint16_t input );
	void setLowerFloatVal( float input );
	void setUpperFloatVal( float input );
	void setLowerIntVal( int16_t input );
	void setUpperIntVal( int16_t input );
	void setLowerUIntVal( uint16_t input );
	void setUpperUIntVal( uint16_t input );
protected:
	uint16_t lowerKnobVal = 0;
	uint16_t upperKnobVal = 1023;
	float lowerFloatVal = -1.0;
	float upperFloatVal = 1.0;
	int16_t lowerIntVal = -32768;
	int16_t upperIntVal = 32767;
	uint16_t lowerUIntVal = 0;
	uint16_t upperUIntVal = 65535;
	int32_t currentValue = 0;

	uint16_t lastRaw = 0;
	
};

//---Hysteresis10BitKnob---------------------------------------------
class Hysteresis10BitKnob_Debug : public Complex10BitKnob_Debug
{
public:
	Hysteresis10BitKnob_Debug( void );
	virtual void freshen( uint16_t msTickDelta );
	void setHysteresis( uint8_t input );
	void setSamplesAveraged( uint8_t input );
protected:
	uint8_t hysteresis = 20;
	int8_t lastSlope = 1;
	uint8_t samplesAveraged = 7;
	CircularBuffer <uint16_t> values;
	CircularBuffer <uint16_t> averages;
	uint8_t hystState = 0; //can be 0 for lower or 1 for output (think positive feedback)
};

//---Windowed10BitKnob---------------------------------------------
class Windowed10BitKnob_Debug : public Complex10BitKnob_Debug
{
public:
	Windowed10BitKnob_Debug( void );
	virtual void freshen( uint16_t msTickDelta );
	void setWindow( int16_t input );
	void setSamplesAveraged( uint8_t input );
	int16_t getWindowUpper( void );
	int16_t getWindowLower( void );
protected:
	void moveWindowUpper( uint16_t input );
	void moveWindowLower( uint16_t input );
	int16_t windowUpper = 60;
	int16_t windowLower = 40;
	int16_t window = 50;
	int8_t lastSlope = 1;
	uint8_t samplesAveraged = 6;
	CircularBuffer <uint16_t> values;
	CircularBuffer <uint16_t> averages;
};
#endif