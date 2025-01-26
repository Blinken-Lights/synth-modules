class WaveTypes extends PApplet {
  //JFrame frame;

  public WaveTypes() {
    super();
    PApplet.runSketch(new String[]{this.getClass().getName()}, this);
  }

  public void settings() {
    size(550, 320);
    smooth();
  }
  public void setup() { 
    windowTitle("Wave Types");
    surface.setLocation(1050, 580);
    
    cp5 = new ControlP5(this);
    
    for(int i = 0; i < 16; i++){
      
      cp5.addRadioButton("waveTypesButton"+i)
             .setId(i)
             .setPosition(0, (i * 20.0))
             .setSize(40,20)
             .setColorLabel(color(waveColour[i]))
             .setColorForeground(color(120))
             .setColorActive(color(waveColour[i]))
             .setItemsPerRow(6)
             .setSpacingColumn(50)
             .addItem("Square"+i,0)
             .addItem("Ramp"+i,1)
             .addItem("InvRamp"+i,2)
             .addItem("Tri"+i,3)
             .addItem("Sine"+i,4)
             .addItem("Exp"+i,5)
             .activate(waveTypes[i])
             ;
             
    }
  }

  public void draw() {
    background(0);
  }
  
  void controlEvent(ControlEvent theEvent) {
    int channel = theEvent.getId();
    int waveType = (int)theEvent.getValue();
    
    waveTypes[channel] = waveType;
    
  }

}
