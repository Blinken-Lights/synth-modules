class FreqMod extends PApplet {
  //JFrame frame;

  public FreqMod() {
    super();
    PApplet.runSketch(new String[]{this.getClass().getName()}, this);
  }

  public void settings() {
    size(850, 320);
    smooth();
  }
  public void setup() { 
    windowTitle("Frequency Modulation");
    surface.setLocation(100, 1000);
    
    cp5 = new ControlP5(this);
    
    for(int i = 0; i < 16; i++){
      
      RadioButton r1 = cp5.addRadioButton("freqModButton"+i)
             .setId(i)
             .setPosition(0, (i * 20.0))
             .setSize(20,20)
             .setColorLabel(color(waveColour[i]))
             .setColorForeground(color(120))
             .setColorActive(color(waveColour[i]))
             .setItemsPerRow(17)
             .setSpacingColumn(30)
             .addItem("FNone"+i,0)
             .addItem("F0-"+i,1)
             .addItem("F1-"+i,2)
             .addItem("F2-"+i,3)
             .addItem("F3-"+i,4)
             .addItem("F4-"+i,5)
             .addItem("F5-"+i,6)
             .addItem("F6-"+i,7)
             .addItem("F7-"+i,8)
             .addItem("F8-"+i,9)
             .addItem("F9-"+i,10)
             .addItem("F10-"+i,11)
             .addItem("F11-"+i,12)
             .addItem("F12-"+i,13)
             .addItem("F13-"+i,14)
             .addItem("F14-"+i,15)
             .addItem("F15-"+i,16)
             .activate(frequencyModulation[i] + 1)
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
    int freqMod = (int)theEvent.getValue() - 1;
    
    frequencyModulation[channel] = freqMod;
    
  }

}
