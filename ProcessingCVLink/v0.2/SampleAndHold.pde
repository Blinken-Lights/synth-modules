class SampleAndHold extends PApplet {
  //JFrame frame;

  public SampleAndHold() {
    super();
    PApplet.runSketch(new String[]{this.getClass().getName()}, this);
  }

  public void settings() {
    size(850, 320);
    smooth();
  }
  public void setup() { 
    windowTitle("Sample And Hold");
    surface.setLocation(80, 900);
    
    cp5 = new ControlP5(this);
    
    for(int i = 0; i < 16; i++){
      
      RadioButton r1 = cp5.addRadioButton("sampleAndHoldButton"+i)
             .setId(i)
             .setPosition(0, (i * 20.0))
             .setSize(20,20)
             .setColorLabel(color(waveColour[i]))
             .setColorForeground(color(120))
             .setColorActive(color(waveColour[i]))
             .setItemsPerRow(17)
             .setSpacingColumn(30)
             .addItem("HNone"+i,0)
             .addItem("H0-"+i,1)
             .addItem("H1-"+i,2)
             .addItem("H2-"+i,3)
             .addItem("H3-"+i,4)
             .addItem("H4-"+i,5)
             .addItem("H5-"+i,6)
             .addItem("H6-"+i,7)
             .addItem("H7-"+i,8)
             .addItem("H8-"+i,9)
             .addItem("H9-"+i,10)
             .addItem("H10-"+i,11)
             .addItem("H11-"+i,12)
             .addItem("H12-"+i,13)
             .addItem("H13-"+i,14)
             .addItem("H14-"+i,15)
             .addItem("H15-"+i,16)
             .activate(sampleAndHoldTrigger[i] + 1)
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
    int sampleTrigger = (int)theEvent.getValue() - 1;
    
    sampleAndHoldTrigger[channel] = sampleTrigger;
    
  }

}
