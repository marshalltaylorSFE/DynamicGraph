#define KNOB_PIN A0

int lastReading;

void setup()
{
	Serial.begin(115200);
	lastReading = analogRead(KNOB_PIN);
}

void loop()
{
	int newReading = analogRead(KNOB_PIN);
	if( lastReading != newReading )
	{
		Serial.println(newReading);
		lastReading = newReading;
	}
	delay(50);
	
}
