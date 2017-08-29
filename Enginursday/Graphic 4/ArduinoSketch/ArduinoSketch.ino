#define KNOB_PIN A0

#define NOISE_RANGE 5

int lastReading;

void setup()
{
	Serial.begin(115200);
	lastReading = analogRead(KNOB_PIN);
}

void loop()
{
	int newReading = analogRead(KNOB_PIN);
	if(( newReading > lastReading + NOISE_RANGE )||( newReading < lastReading - NOISE_RANGE ))
	{
		Serial.println(newReading);
		lastReading = newReading;
	}
	delay(50);
	
}
