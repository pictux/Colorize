 /*
  Colorize is an interactive installation that allows to learn (by playing) how Arduino, 
  conductive paint, capacitive sensor and RGB led strip work.
  Project description / images / Arduino code : 
  https://github.com/pictux/Colorize 
  
  This is a simple sketch to test Arduino & proximity sensors 
    
  This work is licensed under a Creative Commons Attribution-ShareAlike 3.0 
  Unported License (CC BY-SA 3.0) http://creativecommons.org/licenses/by-sa/3.0/
*/

import processing.serial.*;

Serial myPort;  // Create object from Serial class
float red, green, blue;
String longValue = ""; 


void setup() 
{
  size(500, 480);
  // Open whatever port is the one you're using (COMx on Windows, /dev/tty.usbmodem on Mac, /dev/ttyACMx on Linux).
  String portName = "/dev/ttyACM0"; 
  myPort = new Serial(this, portName, 9600);
  myPort.bufferUntil('\n');
}

void draw()
{
  //the background color is defined by the value received from Serial Port
  background(red, green, blue);
  
  rect(((width / 4)) -20, height - 120, 40, - (red*2));
  rect(((width / 4) * 2) -20, height - 120, 40, - (green*2));
  rect(((width / 4) * 3) -20, height - 120, 40, - (blue*2));
}

void serialEvent(Serial p)
{
  longValue = myPort.readString();
  
  red = float(longValue.split(";")[0]);
  green = float(longValue.split(";")[1]);
  blue = float(longValue.split(";")[2]);

  print(red);
  print(";");
  println(green);
  print(";");
  println(blue); 
}
