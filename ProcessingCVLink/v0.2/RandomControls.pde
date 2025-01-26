class RandomControls extends PApplet {
  //JFrame frame;
  
  int[] firstEvents = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  
  public RandomControls() {
    super();
    PApplet.runSketch(new String[]{this.getClass().getName()}, this);
  }

  public void settings() {
    size(700, 320);
    smooth();
  }
  public void setup() { 
    windowTitle("Random Waves");
    surface.setLocation(550, 580);
    
    cp5 = new ControlP5(this);
    
    for(int i = 0; i < 16; i++){
      
      cp5.addSlider("lengthSlider"+i)
       .setId(i)
       .setColorForeground(color(waveColour[i]))
       .setColorActive(color(waveColour[i]))
       .setPosition(0, (i * 20.0))
       .setSize(160,20)
       .setRange(0,32)
       .setValue(euclideanPatternLengths[i][0])
       ;
      
      cp5.addSlider("eventsSlider"+i)
       .setId(16 + i)
       .setColorForeground(color(waveColour[i]))
       .setColorActive(color(waveColour[i]))
       .setPosition(160, (i * 20.0))
       .setSize(160,20)
       .setRange(0,32)
       .setValue(euclideanPatternEvents[i][0])
       ;
      
      cp5.addSlider("offsetsSlider"+i)
       .setId(32 + i)
       .setColorForeground(color(waveColour[i]))
       .setColorActive(color(waveColour[i]))
       .setPosition(320, (i * 20.0))
       .setSize(160,20)
       .setRange(0,32)
       .setValue(euclideanPatternOffsets[i][0])
       ;
       
      cp5.addButton("randomise"+i)
       .setId(48 + i)
       .setValue(0)
       .setPosition(480, (i * 20.0))
       .setSize(50,20)
       ;
       
      cp5.addButton("euclidean"+i)
       .setId(64 + i)
       .setValue(0)
       .setPosition(530, (i * 20.0))
       .setSize(50,20)
       ;
               
    }
  }

  public void draw() {
    background(0);
  }
  
  void controlEvent(ControlEvent theEvent) {
    int controllerType = floor(theEvent.getId() / 16);
    int channel = theEvent.getId() % 16;
    int value = (int)theEvent.getValue();

    if(firstEvents[channel] < 3){
      firstEvents[channel]++;
      return;
    }
    
    if(controllerType == 0){
      euclideanPatternLengths[channel][0] = value;
    }else if(controllerType == 1){
      euclideanPatternEvents[channel][0] = value;
    }else if(controllerType == 2){
      euclideanPatternOffsets[channel][0] = value;
    }else if(controllerType == 3){
      refreshRandomWave(channel, euclideanPatternLengths[channel][0]);
    }else if(controllerType == 4){
      refreshEuclideanWave(channel);
    }
    
  }
  
  public void lengthSlider0(int value){
    
  }
  public void lengthSlider1(int value){
    
  }
  public void lengthSlider2(int value){
    
  }
  public void lengthSlider3(int value){
    
  }
  public void lengthSlider4(int value){
   
  }
  public void lengthSlider5(int value){
    
  }
  public void lengthSlider6(int value){
    
  }
  public void lengthSlider7(int value){
    
  }
  public void lengthSlider8(int value){
    
  }
  public void lengthSlider9(int value){
    
  }
  public void lengthSlider10(int value){
    
  }
  public void lengthSlider11(int value){
    
  }
  public void lengthSlider12(int value){
    
  }
  public void lengthSlider13(int value){
    
  }
  public void lengthSlider14(int value){
    
  }
  public void lengthSlider15(int value){
    
  }
  
  public void eventsSlider0(int value){
    
  }
  public void eventsSlider1(int value){
    
  }
  public void eventsSlider2(int value){
    
  }
  public void eventsSlider3(int value){
    
  }
  public void eventsSlider4(int value){
   
  }
  public void eventsSlider5(int value){
    
  }
  public void eventsSlider6(int value){
    
  }
  public void eventsSlider7(int value){
    
  }
  public void eventsSlider8(int value){
    
  }
  public void eventsSlider9(int value){
    
  }
  public void eventsSlider10(int value){
    
  }
  public void eventsSlider11(int value){
    
  }
  public void eventsSlider12(int value){
    
  }
  public void eventsSlider13(int value){
    
  }
  public void eventsSlider14(int value){
    
  }
  public void eventsSlider15(int value){
    
  }
  
  public void offsetsSlider0(int value){
    
  }
  public void offsetsSlider1(int value){
    
  }
  public void offsetsSlider2(int value){
    
  }
  public void offsetsSlider3(int value){
    
  }
  public void offsetsSlider4(int value){
   
  }
  public void offsetsSlider5(int value){
    
  }
  public void offsetsSlider6(int value){
    
  }
  public void offsetsSlider7(int value){
    
  }
  public void offsetsSlider8(int value){
    
  }
  public void offsetsSlider9(int value){
    
  }
  public void offsetsSlider10(int value){
    
  }
  public void offsetsSlider11(int value){
    
  }
  public void offsetsSlider12(int value){
    
  }
  public void offsetsSlider13(int value){
    
  }
  public void offsetsSlider14(int value){
    
  }
  public void offsetsSlider15(int value){
    
  }

}
