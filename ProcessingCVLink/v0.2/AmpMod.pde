class AmpMod extends PApplet {
  //JFrame frame;

  public AmpMod() {
    super();
    PApplet.runSketch(new String[]{this.getClass().getName()}, this);
  }

  public void settings() {
    size(850, 320);
    smooth();
  }
  public void setup() { 
    windowTitle("Amplitude Modulation");
    surface.setLocation(20, 800);
    
    cp5 = new ControlP5(this);
    
    for(int i = 0; i < 16; i++){
      
      RadioButton r1 = cp5.addRadioButton("ampModButton"+i)
             .setId(i)
             .setPosition(0, (i * 20.0))
             .setSize(20,20)
             .setColorLabel(color(waveColour[i]))
             .setColorForeground(color(120))
             .setColorActive(color(waveColour[i]))
             .setItemsPerRow(17)
             .setSpacingColumn(30)
             .addItem("ANone"+i,0)
             .addItem("A0-"+i,1)
             .addItem("A1-"+i,2)
             .addItem("A2-"+i,3)
             .addItem("A3-"+i,4)
             .addItem("A4-"+i,5)
             .addItem("A5-"+i,6)
             .addItem("A6-"+i,7)
             .addItem("A7-"+i,8)
             .addItem("A8-"+i,9)
             .addItem("A9-"+i,10)
             .addItem("A10-"+i,11)
             .addItem("A11-"+i,12)
             .addItem("A12-"+i,13)
             .addItem("A13-"+i,14)
             .addItem("A14-"+i,15)
             .addItem("A15-"+i,16)
             .activate(amplitudeModulation[i] + 1)
             ;
       
       int j = 0;
       for(Toggle t:r1.getItems()) {
         if(j == i + 1){
           t.setColorBackground(color(0));
           t.setColorForeground(color(0));
           t.setColorActive(color(0));
         }else if(j > 0){
           t.setColorBackground(color(waveColour[j-1]));
         }
         j++;
       }
    }
  }

  public void draw() {
    background(0);
  }
  
  void controlEvent(ControlEvent theEvent) {
    int channel = theEvent.getId();
    int ampMod = (int)theEvent.getValue() - 1;
    
    amplitudeModulation[channel] = ampMod;
    
  }

}
