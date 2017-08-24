//---Hysteresis10BitKnob--------------------------------------------------------

	if(hystState == 0)
	{
		if( input > currentValue )
		{
			//Allow current to go up only
			currentValue = input;
			newData = 1;
		}
		if( input < currentValue - hysteresis )
		{
			//toggle behavior
			hystState = 1;
			currentValue = input;
			newData = 1;			
		}
	}
	else
	{
		if( input < currentValue )
		{
			//Allow current to go up only
			currentValue = input;
			newData = 1;
		}
		if( input > currentValue + hysteresis )
		{
			//toggle behavior
			hystState = 0;
			currentValue = input;
			newData = 1;			
		}
	}
	







	
	
	
	
	
	
	
	
	
	
	
	
	
	



//---Windowed10BitKnob--------------------------------------------------------

	if( input > windowUpper )
	{
		windowUpper = input;
		windowLower = input - window;
		newData = 1;
	}
	if( input < windowLower )
	{
		windowLower = input;
		windowUpper = input + window;
		newData = 1;
	}

	uint32_t temp = windowLower + (window >> 1);
	//scale it
	currentValue = ((temp - (window >> 1)) << 10 ) / (1024 - window );
}

























