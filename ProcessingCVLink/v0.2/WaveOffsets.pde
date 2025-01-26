class WaveOffsets extends PApplet {
  //JFrame frame;
  
  int[] firstEvents = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  
  public WaveOffsets() {
    super();
    PApplet.runSketch(new String[]{this.getClass().getName()}, this);
  }

  public void settings() {
    size(550, 320);
    smooth();
  }
  public void setup() { 
    windowTitle("Wave Offsets");
    surface.setLocation(350, 580);
    
    cp5 = new ControlP5(this);
    
    for(int i = 0; i < 16; i++){
      
      cp5.addSlider("waveOffsetsSlider"+i)
       .setId(i)
       .setColorForeground(color(waveColour[i]))
       .setColorActive(color(waveColour[i]))
       .setPosition(0, (i * 20.0))
       .setSize(550,20)
       .setRange(-4095,4095)
       .setValue(waveOffsets[i])
       ;
             
    }
  }

  public void draw() {
    background(0);
  }
  
  void controlEvent(ControlEvent theEvent) {
    int channel = theEvent.getId();
    float waveOffset = (float)theEvent.getValue();

    if(firstEvents[channel] < 3){
      firstEvents[channel]++;
      return;
    }
    
    waveOffsets[channel] = waveOffset;
    
  }

}
