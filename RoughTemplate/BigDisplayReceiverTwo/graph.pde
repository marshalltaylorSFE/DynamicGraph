

class pGraph
{
	pGraph()
	{
		rawInput = new GraphTrace();
		randomKnob = new GraphTrace();
		randomKnob.traceColor[0] = 0;
		randomKnob.traceColor[1] = 0;
		randomKnob.traceColor[2] = 255;
		eventStream = new GraphTrace();
		eventStream.y_scale = 20;
		background = createShape(RECT, 0, 0, x_size, y_size);
		background.setFill(color(255, 255, 255));
		background.setStroke(false);
	}
	void drawBackground()
	{
		shape(background, x_origin, y_origin);
		stroke(0,0,0);
		strokeWeight(2);		
		if( x_axis_on == 1 )
		{
			line(x_origin, y_origin + y_size, x_origin + x_size, y_origin + y_size);
			if( x_ticks > 0 )
			{
				for( int i = x_origin; i < x_origin + x_size; i = i + x_ticks )
				{
					line(i, y_origin + y_size, i, y_origin + y_size + 5);
				}
			}		}
		if( y_axis_on == 1 )
		{
			line(x_origin, y_origin, x_origin, y_origin + y_size);
			if( y_ticks > 0 )
			{
			    textSize(20);
				for( int i = y_origin + y_size; i > y_origin; i = i - y_ticks )
				{
					line(x_origin, i, x_origin - 5, i);
					String outputString = str(y_origin + y_size - i);
					text(outputString, x_origin - 10, i + 7);
				}
			}
		}
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
		rawInput.postDraw();
		randomKnob.postDraw();
		eventStream.postDraw();
	}
	PShape background;
	GraphTrace rawInput;
	GraphTrace randomKnob;
	GraphTrace eventStream;
	
	int x_origin = 140;
	int y_origin = 316;
	int x_size = 540;
	int y_size = 286;
	int x_axis_on = 1;
	int y_axis_on = 1;
	int x_ticks = 100;
	int y_ticks = 32;

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
		int dataPtr2;

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
			line(x_origin + i, y_origin + y_size - (data[dataPtr1] * y_scale), x_origin + i + 1, y_origin + y_size - (data[dataPtr2] * y_scale));
			//increment dataPtr1
			dataPtr1++;
	
			
		}
	}
	int x_origin = 140;
	int y_origin = 316;
	int x_size = 540;
	int y_size = 286;
	float x_scale = 1.0;
	float y_scale = 1.0;
	int newestDataPtr = 0;
	int[] traceColor = {255, 0, 0};
	
	int[] data;
	
}