import processing.serial.*;
import java.util.*;

// The serial port:
Serial myPort;     
uCPacketUART mySerialReceiver;

int displayValue = 0;
int diagramOneOldestIndex = 0;
PImage sevenSegmentBackground;  // Declare a variable of type PImage
PImage[] sevenSegment = new PImage[2];
PFont font;

PShape segmentBox;  // The PShape object
PShape termBox;  // The PShape object
PShape serialBox;  // The PShape object

pGraph myGraph;
pLogicGraph myLogicGraph;

int[] debugData;
int debugDataLatestPtr;

float yPos = 0.0;

void setup() {  // setup() runs once
	printArray(Serial.list());

	// Open the port you are using at the rate you want:
	myPort = new Serial(this, Serial.list()[Serial.list().length - 1], 250000);
	mySerialReceiver = new uCPacketUART(20);
	myGraph = new pGraph();
	myLogicGraph = new pLogicGraph();
	
	debugData = new int[20];
	for(int i = 0; i < 20; i++)
	{
		debugData[i] = 0;
	}
	debugDataLatestPtr = 0;

	
	sevenSegmentBackground = loadImage("alloff.png");
	sevenSegment[0] = loadImage("font0.png");
	sevenSegment[1] = loadImage("font1.png");
	segmentBox = createShape(RECT, 0, 0, 540, 113);
	segmentBox.setFill(color(0, 0, 0));
	segmentBox.setStroke(false);

	termBox = createShape(RECT, 0, 0, 540, 113);
	termBox.setFill(color(255, 255, 255));
	termBox.setStroke(false);

	serialBox = createShape(RECT, 0, 0, 900, 200);
	serialBox.setFill(color(255, 255, 255));
	serialBox.setStroke(false);

	// The font must be located in the sketch's 
	// "data" directory to load successfully
	font = createFont("courbd.ttf", 80);
	textFont(font);


	size(960, 1080);
	frameRate(100);

	Timer time = new Timer(); // Instantiate Timer Object
	CustomTask st = new CustomTask(); // Instantiate SheduledTask class
	time.schedule(st, 0, 3000); // Create Repetitively task for every 1 secs


}
float framecounter = 0;
void draw() {  // draw() loops forever, until stopped
	mySerialReceiver.doathing();
	mySerialReceiver.parseSingle();
	displayValue = (int)(mySerialReceiver.rawADC);
	//myGraph.y_ticks = (int)(mySerialReceiver.knob2)/2 + 16;
	//myGraph.rawInput.y_scale = pow(10, ((float)(mySerialReceiver.rawADC - 5))/4);

	myGraph.rawInput.write(displayValue);

	myGraph.randomKnob.write((int)mySerialReceiver.filteredADC);

	myGraph.var1 = (int)mySerialReceiver.var1;
	myGraph.var2 = (int)mySerialReceiver.var2;
	
	myLogicGraph.eventStream.write((int)mySerialReceiver.event);
	background(242,230,210);

	//drawTrash();

	//Draw framy stuff
	textAlign(LEFT);
	strokeWeight(3);
	stroke(0,0,0);
	textSize(40);
	String outputString = "Raw ADC Data";
	text(outputString, 30, 50);
	line(0,45,20,45);
	line(330,45,960,45);
	textSize(30);
	textAlign(RIGHT);
	outputString = "Binary:";
	text(outputString, 370, 140);
	outputString = "Decimal:";
	text(outputString, 370, 270);
	textSize(40);
	textAlign(LEFT);
	outputString = "ADC Logic Graphs";
	text(outputString, 30, 350);
	line(0,345,20,345);
	line(425,345,960,345);
	outputString = "Serial Debug";
	text(outputString, 30, 800);
	line(0,795,20,795);
	line(330,795,960,795);	
	drawSegmentBox(390, 60, displayValue);
	//legend
	textAlign(LEFT);
	strokeWeight(3);
	textSize(30);

	stroke(255,0,0);
	line(50,430,100,430);
	outputString = "Raw";
	text(outputString, 110, 435);	

	stroke(27, 106, 209);
	line(50,460,100,460);
	outputString = "Filtered";
	text(outputString, 110, 465);		

	stroke(102, 191, 1);
	line(50,490,100,490);
	outputString = "Threshold";
	text(outputString, 110, 495);		

	outputString = "Filter Type:";
	text(outputString, 25, 600);
	textSize(24);
	outputString = str(myGraph.var1 - myGraph.var2) + " pt. window";
	text(outputString, 35, 630);
	outputString = "";
	text(outputString, 35, 660);
	
	drawTermBox(390, 190, displayValue);

	drawFakeSerialBox(30, 825);
	
	myGraph.drawAll();
	myLogicGraph.drawAll();

	framecounter++;
}

void drawSegmentBox( int x, int y, int data ) {
	shape(segmentBox, x, y);
	image(sevenSegmentBackground, x + 20, y + 20);
	image(sevenSegmentBackground, x + 70, y + 20);
	image(sevenSegmentBackground, x + 120, y + 20);
	image(sevenSegmentBackground, x + 170, y + 20);
	image(sevenSegmentBackground, x + 220, y + 20);
	image(sevenSegmentBackground, x + 270, y + 20);
	image(sevenSegmentBackground, x + 320, y + 20);
	image(sevenSegmentBackground, x + 370, y + 20);
	image(sevenSegmentBackground, x + 420, y + 20);
	image(sevenSegmentBackground, x + 470, y + 20);

	image(sevenSegment[(data >> 9)&0x0001], x + 20, y + 20);
	image(sevenSegment[(data >> 8)&0x0001], x + 70, y + 20);
	image(sevenSegment[(data >> 7)&0x0001], x + 120, y + 20);
	image(sevenSegment[(data >> 6)&0x0001], x + 170, y + 20);
	image(sevenSegment[(data >> 5)&0x0001], x + 220, y + 20);
	image(sevenSegment[(data >> 4)&0x0001], x + 270, y + 20);
	image(sevenSegment[(data >> 3)&0x0001], x + 320, y + 20);
	image(sevenSegment[(data >> 2)&0x0001], x + 370, y + 20);
	image(sevenSegment[(data >> 1)&0x0001], x + 420, y + 20);
	image(sevenSegment[(data)&0x0001], x + 470, y + 20);

}

void drawTermBox( int x, int y, int data ) {
	shape(termBox, x, y);
	fill(0, 0, 0);
	textAlign(RIGHT);
	//String outputString = "0x" + hex(displayValue, 3);
	String outputString = str(displayValue);
	textSize(80);
	text(outputString, x + 510, y + 83);
}

void drawFakeSerialBox( int x, int y )
{
	shape(serialBox, x, y);
	String s = "";
	int dataPtr = 0;
	for(int i = 0; i < 7; i++)
	{
		dataPtr = debugDataLatestPtr - 7 + i;
		if(dataPtr < 0) dataPtr = dataPtr + 20;
		s = s + str(debugData[dataPtr]) + "\n";
	}
	fill(50);
	textSize(18);
	textAlign(LEFT);
	text(s, x + 30, y + 10, 250, 300);  // Text wraps within text box
}

void drawTrash() {
	yPos = yPos - 1.0;
	if (yPos < - 200) {
		yPos = height;
	}
	stroke(119,83,44);
	strokeWeight(3);
	line(0, yPos, width, yPos);
	stroke(84,60,34);
	strokeWeight(1);
	line(0, yPos + 180, width, yPos + 180);
}


public class CustomTask extends TimerTask  {

	public CustomTask(){
		//Constructor
	}

	public void run() {
		try {
			// Your task process
			System.out.print("frameRate = ");
			System.out.println(frameRate);
		} catch (Exception ex) {

			System.out.println("error running thread " + ex.getMessage());
		}
	}
}