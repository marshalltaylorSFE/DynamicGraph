

class pGraph
{
	pGraph()
	{
		rawInput = new GraphTrace();
		randomKnob = new GraphTrace();
		randomKnob.traceColor[0] = 0;
		randomKnob.traceColor[1] = 0;
		randomKnob.traceColor[2] = 255;
		background = createShape(RECT, 0, 0, x_size, y_size);
		background.setFill(color(255, 255, 255));
		background.setStroke(false);
	}
	void drawBackground()
	{
		shape(background, x_origin, y_origin);
		stroke(0,0,0);
		strokeWeight(2);		
		textAlign(RIGHT);
		if( x_axis_on == 1 )
		{
			line(x_origin, y_origin + y_size, x_origin + x_size, y_origin + y_size);
			if( x_ticks > 0 )
			{
				for( int i = x_origin; i < x_origin + x_size; i = i + x_ticks )
				{
					line(i, y_origin + y_size, i, y_origin + y_size + 5);
				}
			}
		}
		if( y_axis_on == 1 )
		{
			line(x_origin, y_origin, x_origin, y_origin + y_size);
			if( y_ticks > 0 )
			{
			    textSize(20);
				for( int i = 0; i < 1024; i= i + y_ticks )
				{
					int y2 = y_origin + y_size - (int)((i-y_offset) * y_scale);
					if(( y2 > y_origin )&&( y2 < y_origin + y_size ))
					{
						stroke(0,0,0);
						strokeWeight(2);
						line(x_origin, y2, x_origin - 5, y2);
						stroke(200,200,200);
						strokeWeight(1);
						line(x_origin, y2, x_origin + x_size, y2);
						String outputString = str(i);
						text(outputString, x_origin - 10, y2 + 7);
					}
				}

			}
		}
	}
	void drawAll()
	{
		y_offset = rawInput.new_y_offset;
		rawInput.y_offset = y_offset;
		randomKnob.y_offset = y_offset;
		preDraw();
		drawBackground();
		fill(0, 0, 0);
		postDraw();
	}
	void preDraw()
	{
	}
	void postDraw()
	{
		rawInput.postDraw();
		//randomKnob.postDraw();
	}
	PShape background;
	GraphTrace rawInput;
	GraphTrace randomKnob;
	GraphTrace eventStream;
	
	int x_origin = 390;
	int y_origin = 390;
	int x_size = 540;
	int y_size = 257;
	int x_axis_on = 1;
	int y_axis_on = 1;
	int x_ticks = 100;
	int y_ticks = 32;
	float y_scale = 2.0;
	int y_offset = 512;
};

class GraphTrace
{
	GraphTrace()
	{
		data = new int[x_size];
		for( int i = 0; i < x_size; i++ )
		{
			data[i] = 0;
		}
	}
	void write( int input )
	{
		newestDataPtr = newestDataPtr + 1;
		if( newestDataPtr >= x_size )
		{
			newestDataPtr = newestDataPtr - x_size;
		}
		data[newestDataPtr] = input;
	}
	void preDraw()
	{
	}
	void postDraw()
	{
		stroke(traceColor[0],traceColor[1],traceColor[2]);
		strokeWeight(3);

		int dataPtr1;
		int dataPtr2 = 0;

		for( int i = 0; i < x_size - 40; i++ )
		{
			//Calculate points left to right
			dataPtr1 = newestDataPtr - (x_size - 40) + i;
			if( dataPtr1 < 0 )
			{
				dataPtr1 = dataPtr1 + x_size;
			}			
			dataPtr2 = dataPtr1 + 1;
			if( dataPtr2 >= x_size )
			{
				dataPtr2 = 0;
			}
			int x1 = x_origin + i;
			int y1 = y_origin + y_size - (int)((data[dataPtr1]-y_offset) * y_scale);
			if(y1 < y_origin)
			{
				y1 = y_origin;
			}
			else if(y1 > y_origin + y_size)
			{
				y1 = y_origin + y_size;
			}
			int x2 = x_origin + i + 1;
			int y2 = y_origin + y_size - (int)((data[dataPtr2]-y_offset) * y_scale);
			if(y2 < y_origin)
			{
				y2 = y_origin;
			}
			else if(y2 > y_origin + y_size)
			{
				y2 = y_origin + y_size;
			}
			
			
			line(x1, y1, x2, y2);
			//increment dataPtr1
			dataPtr1++;
	
			
		}
		//Calc new offsets based of last point
		int y2 = y_origin + y_size - (int)((data[dataPtr2]-y_offset) * y_scale);
		if(y2 < y_origin)
		{
			//It's too high
			//new_y_offset = (int)((data[dataPtr2]-y_offset) * y_scale) - y_size;
			new_y_offset = (data[dataPtr2])- (int)(y_size / y_scale);
			//if(new_y_offset > (1023-y_size)) new_y_offset = (1023-y_size);
			System.out.println("Too high!");
			System.out.print("  Data:");
			System.out.println(data[dataPtr2]);
			System.out.print("  Offset:");
			System.out.println(y_offset);
			System.out.print("  New offset:");
			System.out.println(new_y_offset);
		}
		else if(y2 > y_origin + y_size)
		{
			//It's too low
			//new_y_offset = (int)((data[dataPtr2]-y_offset) * y_scale);
			new_y_offset = data[dataPtr2];
			if(new_y_offset < 0) new_y_offset = 0;
			System.out.print("Too low, new offset = ");
			System.out.println(new_y_offset);
		}

	}
	int x_origin = 390;
	int y_origin = 390;
	int x_size = 540;
	int y_size = 257;
	float x_scale = 1.0;
	float y_scale = 2.0;
	int y_offset = 512;
	int newestDataPtr = 0;
	int[] traceColor = {255, 0, 0};
	
	int[] data;
	int new_y_offset = 512;
	
}

class pLogicGraph
{
	pLogicGraph()
	{
		eventStream = new GraphTrace();
		eventStream.x_origin = x_origin;
		eventStream.y_origin = y_origin;
		eventStream.x_size = x_size;
		eventStream.y_size = y_size;
		eventStream.y_scale = 32;
		eventStream.y_offset = 0;
		eventStream.traceColor[0] = 121;
		eventStream.traceColor[1] = 25;
		eventStream.traceColor[2] = 211;		background = createShape(RECT, 0, 0, x_size, y_size);
		background.setFill(color(255, 255, 255));
		background.setStroke(false);
	}
	void drawBackground()
	{
		shape(background, x_origin, y_origin);
		stroke(0,0,0);
		strokeWeight(2);
		//Draw axes
		line(x_origin, y_origin + y_size + 2, x_origin + x_size, y_origin + y_size + 2);

		for( int i = x_origin; i < x_origin + x_size; i = i + 100 )
		{
			line(i, y_origin + y_size + 2, i, y_origin + y_size + 5 + 2);
		}
		
		line(x_origin, y_origin, x_origin, y_origin + y_size);

		textSize(20);

		line(x_origin, y_origin + y_size, x_origin - 5, y_origin + y_size);
		String outputString = "Idle";
		text(outputString, x_origin - 10, y_origin + y_size + 7);

		line(x_origin, y_origin + y_size - 32, x_origin - 5, y_origin + y_size - 32);
		outputString = "Triggered";
		text(outputString, x_origin - 10, y_origin + y_size + 7 - 32 - 2);
		stroke(200,200,200);
		strokeWeight(1);
		line(x_origin, y_origin + y_size - 32, x_origin + x_size, y_origin + y_size - 32);
	}
	void drawAll()
	{
		preDraw();
		drawBackground();
		fill(0, 0, 0);
		postDraw();
	}
	void preDraw()
	{
	}
	void postDraw()
	{
		
		eventStream.postDraw();

	}
	PShape background;
	GraphTrace eventStream;
	
	int x_origin = 390;
	int y_origin = 700;
	int x_size = 540;
	int y_size = 48;

};