class WaveSpeeds extends PApplet {
  //JFrame frame;
  
  int[] firstEvents = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  
  public WaveSpeeds() {
    super();
    PApplet.runSketch(new String[]{this.getClass().getName()}, this);
  }

  public void settings() {
    size(700, 320);
    smooth();
  }
  public void setup() { 
    windowTitle("Wave Speeds");
    surface.setLocation(550, 580);
    
    cp5 = new ControlP5(this);
    
    for(int i = 0; i < 16; i++){
      
      cp5.addSlider("waveSpeedsSlider"+i)
       .setId(i)
       .setColorForeground(color(waveColour[i]))
       .setColorActive(color(waveColour[i]))
       .setPosition(0, (i * 20.0))
       .setSize(550,20)
       .setRange(0,255) // .setRange(0,255) // up to 255 for non-serial speeds
       .setValue(waveIncrements[i])
       ;
             
    }
  }

  public void draw() {
    background(0);
  }
  
  void controlEvent(ControlEvent theEvent) {
    int channel = theEvent.getId();
    float waveIncrement = (float)theEvent.getValue();

    if(firstEvents[channel] < 3){
      firstEvents[channel]++;
      return;
    }
    
    waveIncrements[channel] = waveIncrement;
    
  }

}
