#define KNOB_PIN A0

void setup()
{
	Serial.begin(115200);
	
}

void loop()
{
	Serial.println(analogRead(KNOB_PIN));
	
	delay(50);
	
}
