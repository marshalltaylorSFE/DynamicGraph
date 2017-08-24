//This file specifies the packets used by the application.
//These are structures the must be transmitted and recieved
//As a stream of bytes somehow.
#ifndef USERPACKETDEFS_H
#define USERPACKETDEFS_H
struct robotClientPacket
{
	uint8_t packetStatus;
	uint8_t gamepadButtons;
	uint32_t testHex;
};

struct robotHostPacket
{
	uint8_t packetStatus;
	
};

struct knobPacket
{
	uint16_t knob1;
	uint16_t knob2;
	uint16_t knob3;
	uint16_t knob4;
	uint16_t knob5;
	uint16_t packetNumber;
};

#endif