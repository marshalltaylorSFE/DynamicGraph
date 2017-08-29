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


























