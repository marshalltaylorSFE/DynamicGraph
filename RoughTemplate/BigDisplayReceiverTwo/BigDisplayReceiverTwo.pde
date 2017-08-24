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

pGraph myGraph;

float yPos = 0.0;

void setup() {  // setup() runs once
	printArray(Serial.list());

	// Open the port you are using at the rate you want:
	myPort = new Serial(this, Serial.list()[Serial.list().length - 1], 250000);
	mySerialReceiver = new uCPacketUART(20);
	myGraph = new pGraph();

	sevenSegmentBackground = loadImage("alloff.png");
	sevenSegment[0] = loadImage("font0.png");
	sevenSegment[1] = loadImage("font1.png");
	segmentBox = createShape(RECT, 0, 0, 540, 113);
	segmentBox.setFill(color(0, 0, 0));
	segmentBox.setStroke(false);

	termBox = createShape(RECT, 0, 0, 540, 113);
	termBox.setFill(color(255, 255, 255));
	termBox.setStroke(false);



	// The font must be located in the sketch's 
	// "data" directory to load successfully
	font = createFont("courbd.ttf", 80);
	textFont(font);


	size(720, 632);
	frameRate(100);

	Timer time = new Timer(); // Instantiate Timer Object
	CustomTask st = new CustomTask(); // Instantiate SheduledTask class
	time.schedule(st, 0, 3000); // Create Repetitively task for every 1 secs


}
float framecounter = 0;
void draw() {  // draw() loops forever, until stopped
	mySerialReceiver.doathing();
	mySerialReceiver.parseSingle();
	displayValue = (int)(mySerialReceiver.knob4);
	myGraph.y_ticks = (int)(mySerialReceiver.knob2)/2 + 16;
	myGraph.rawInput.y_scale = pow(10, ((float)(mySerialReceiver.knob1 - 5))/4);
	myGraph.rawInput.write(displayValue);

	myGraph.randomKnob.write((int)mySerialReceiver.knob5);

	background(242,230,210);

	drawTrash();

	drawSegmentBox(140, 30, displayValue);

	drawTermBox(140, 173, displayValue);

	myGraph.drawAll();

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
	String outputString = "0x" + hex(displayValue, 3);
	textSize(80);
	text(outputString, x + 510, y + 83);
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