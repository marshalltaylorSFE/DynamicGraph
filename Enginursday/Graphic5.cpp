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