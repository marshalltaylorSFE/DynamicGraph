

class uCPacketUART
{
	uCPacketUART( int bytesAllocated )
	{
	}
	void doathing(  )
	{
		while ( myPort.available() > 0) {  // If data is available,
			char c = (char)myPort.read();
			sBuffer.append(c);         // read it and store it in val
			if( sBuffer.length() > 1000)
			{
				//print(sBuffer.substring(0, 22));
				sBuffer.delete(1000, sBuffer.length());
			}
		}
	}
	int parseSingle()  //parse and remove all valid packets, keep newest
	{
		int returnVar = 0;
		//Look for the first ~ or end of string
		int index = sBuffer.indexOf("~", 0);
		if( index == -1 )
		{
			//start not found
			returnVar = -1;
			//println("NF1");
		}
		else
		{
			//found, strip begining
			sBuffer.delete(0, index);
			//look for complete packet by finding next
			index = sBuffer.indexOf("\n", 0);
			if(( index != -1 )&&(sBuffer.length() > 25))
			{
				//We found a full line, remove it
				StringBuffer pBuffer = new StringBuffer(sBuffer.substring(0, index));
				//println(pBuffer.length());
				//println(pBuffer);
				sBuffer.delete(0, index);
				//println(pBuffer);
				rawADC = unhex(pBuffer.substring(3,5) + pBuffer.substring(1,3));
				filteredADC = unhex(pBuffer.substring(7,9) + pBuffer.substring(5,7));
				var1 = unhex(pBuffer.substring(11,13) + pBuffer.substring(9,11));
				var2 = unhex(pBuffer.substring(15,17) + pBuffer.substring(13,15));
				event = unhex(pBuffer.substring(19,21) + pBuffer.substring(17,19));
				packetNumber = unhex(pBuffer.substring(23,25) + pBuffer.substring(21,23));
				
				//Load serial window
				if( event != 0 )
				{
					debugDataLatestPtr++;
					if(debugDataLatestPtr >= 20) debugDataLatestPtr = 0;
					debugData[debugDataLatestPtr] = filteredADC;
				}
				
			}
		}
		return returnVar;
	}
	
	//knobPacket rxData = new knobPacket();
	
	StringBuffer sBuffer = new StringBuffer("test");
	
	public int rawADC = 0;
	public int filteredADC = 0;
	public int var1 = 0;
	public int var2 = 0;
	public int event = 0;
	public int packetNumber = 0;	
	
	char startSymbol = '~';
};

