import controlP5.*;
import java.time.Instant;
import java.util.concurrent.TimeUnit;
import processing.serial.*;

Square square;
Ramp ramp;
RampInv rampInv;
Tri tri;
Sine sine;
Exp exp;

ControlP5 cp5;
Slider abc;

boolean serialOutput = true;

Serial myPort;  // Create object from Serial class
int val;        // Data received from the serial port
boolean firstContact = false;

int speed = 750;
long lastThreadBeat = 0;

long t = 0;

float[] waveIndexes = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float[] lastDisplayedWaveIndexes = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float[] waveIncrements = { 1.0, 1.1, 1.5, 1.7, 2.0, 0.25, 12, 4, 0.09, 0.08, 0.07, 0.06, 0.05, 0.04, 0.03, 0.02 };
color[] waveColour = {
  color(255, 0, 0),
  color(0, 255, 0),
  color(0, 0, 255),
  color(255, 0, 255),
  color(0, 255, 255),
  color(255, 255, 0),
  color(127, 0, 0),
  color(0, 127, 0),
  color(255, 0, 127),
  color(127, 255, 0),
  color(127, 0, 255),
  color(255, 0, 255),
  color(127, 255, 255),
  color(255, 255, 127),
  color(127, 127, 0),
  color(0, 127, 127),
};

int[] waveTypes = {
  0,
  1,
  2,
  3,
  4,
  3,
  5,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3
};

// for debugging / test display
long lastDisplayedT = 0;
int displayedWidth = 0;

int buffersSent = 0;

void setup() {
  
  if(serialOutput){
    String portName = Serial.list()[2];
    println(portName);
    myPort = new Serial(this, portName, 2000000);
  }

  size(1500, 600);
  noStroke();
  background(0);

  cp5 = new ControlP5(this);

  int posY = 542;

  cp5.addSlider("s")
    .setSize(472, 40)
    .setPosition(20, posY)
    .setRange(1, 500) // 3 (ie 750 us) seems to work best
    .setValue(3)
    ;
    
  square = new Square();
  ramp = new Ramp();
  rampInv = new RampInv();
  tri = new Tri();
  sine = new Sine();
  exp = new Exp();
    
  //thread("betterTiming");
  
}

/*void betterTiming(){
  
  while (true) {
    
    long m = micros(); // Instant.now().toEpochMilli(); //millis();
  
    if(m - lastThreadBeat >= speed){
      if(serialOutput){
        String dataString = "";
        for(int i = 0; i < 8; i++){
          dataString+= str(4095 - round(waveIndexes[i])) + ",";
        }
        dataString+="\n";
        myPort.write(dataString);
      }
      
      if((m - lastThreadBeat) > speed){
        float error = abs((m - lastThreadBeat) - speed);
        println((m - lastThreadBeat) + "\t" + speed + "\t" + error);
      }
      
      t++;
      for(int i = 0; i < 16; i++){
        waveIndexes[i]+= waveIncrements[i];
        if(waveIndexes[i] > 4095){
          waveIndexes[i] = 0;
        }
      }
      
      lastThreadBeat = m;
    }
    
  }
  
}*/

void serialEvent(Serial myPort) {
  // read a byte from the serial port:
  int inByte = myPort.read();
  // if this is the first byte received, and it's an A,
  // clear the serial buffer and note that you've
  // had first contact from the microcontroller. 
  // Otherwise, add the incoming byte to the array:
  
  println("received serial: " + inByte + "\t" + buffersSent);
  
  buffersSent++;

  if (inByte == 'A') {
    
    myPort.clear();          // clear the serial port buffer
    firstContact = true;     // you've had first contact from the microcontroller
    myPort.write('C');
    
  }else if (inByte == 'B') {
    
    myPort.clear();
    
    // send 1.3s of data
    // max buffer size on the Pico is 14000 - so 13000 is about the maximum that can be sent at one time
    // without overwriting the place that's being played
    // 1000 seems to work well here, with the Arduino requesting more data at 500 remaining samples
    for(int m = 0; m < 1000; m++){
      String dataString = "";
      for(int i = 0; i < 8; i++){
        //dataString+= round(waveIndexes[i]) + ",";
        switch (waveTypes[i]){
          case 0:
            dataString+= square.waveTable[round(waveIndexes[i])] + ",";
            break;
          case 1:
            dataString+= ramp.waveTable[round(waveIndexes[i])] + ",";
            break;
          case 2:
            dataString+= rampInv.waveTable[round(waveIndexes[i])] + ",";
            break;
          case 3:
            dataString+= tri.waveTable[round(waveIndexes[i])] + ",";
            break;
          case 4:
            dataString+= sine.waveTable[round(waveIndexes[i])] + ",";
            break;
          case 5:
            dataString+= exp.waveTable[round(waveIndexes[i])] + ",";
            break;
        }
      }
      dataString+="\n";
      myPort.write(dataString);
      
      t++;
      for(int i = 0; i < 16; i++){
        waveIndexes[i]+= waveIncrements[i];
        if(waveIndexes[i] > 4095){
          waveIndexes[i] = 0;
        }
      }
    }
    myPort.write("\t");
    
  }
  
}

void draw() {
  
  /*stroke(255);
  line(displayedWidth, 512 - (lastDisplayedT % 512), displayedWidth, 512 - (t % 512));
  
  lastDisplayedT = t;*/
  
  for(int i = 0; i < 16; i++){
    
    int newWavePoint = 0;
    
    switch (waveTypes[i]){
      case 0:
        newWavePoint = square.waveTable[round(waveIndexes[i])];
        break;
      case 1:
        newWavePoint = ramp.waveTable[round(waveIndexes[i])];
        break;
      case 2:
        newWavePoint = rampInv.waveTable[round(waveIndexes[i])];
        break;
      case 3:
        newWavePoint = tri.waveTable[round(waveIndexes[i])];
        break;
      case 4:
        newWavePoint = sine.waveTable[round(waveIndexes[i])];
        break;
      case 5:
        newWavePoint = exp.waveTable[round(waveIndexes[i])];
        break;
    }
    
    stroke(waveColour[i]);
    
    line(displayedWidth, 512 - (round(lastDisplayedWaveIndexes[i]) >> 3), displayedWidth, 512 - (newWavePoint >> 3));
    
    lastDisplayedWaveIndexes[i] = newWavePoint;
    
  }
  
  displayedWidth++;
  if(displayedWidth > 1500){
    displayedWidth = 0;
    background(0);
  }

}

void s(int sV) {
  speed = (sV * 250);
}

// based on https://stackoverflow.com/questions/47869659/convert-instant-to-microseconds-from-epoch-time
long micros(){
  
  Instant inst = Instant.now();
  // Nano seconds since epoch, may overflow
  //long nanos = TimeUnit.SECONDS.toNanos(inst.getEpochSecond()) + inst.getNano();
  // Microseconds since epoch, may overflow
  long micros = TimeUnit.SECONDS.toMicros(inst.getEpochSecond()) + TimeUnit.NANOSECONDS.toMicros(inst.getNano());
  
  return micros;
  
}
