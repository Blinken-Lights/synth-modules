class Sync extends PApplet {
  //JFrame frame;

  public Sync() {
    super();
    PApplet.runSketch(new String[]{this.getClass().getName()}, this);
  }

  public void settings() {
    size(850, 320);
    smooth();
  }
  public void setup() { 
    windowTitle("Sync");
    surface.setLocation(40, 850);
    
    cp5 = new ControlP5(this);
    
    for(int i = 0; i < 16; i++){
      
      RadioButton r1 = cp5.addRadioButton("syncButton"+i)
             .setId(i)
             .setPosition(0, (i * 20.0))
             .setSize(20,20)
             .setColorLabel(color(waveColour[i]))
             .setColorForeground(color(120))
             .setColorActive(color(waveColour[i]))
             .setItemsPerRow(17)
             .setSpacingColumn(30)
             .addItem("SNone"+i,0)
             .addItem("S0-"+i,1)
             .addItem("S1-"+i,2)
             .addItem("S2-"+i,3)
             .addItem("S3-"+i,4)
             .addItem("S4-"+i,5)
             .addItem("S5-"+i,6)
             .addItem("S6-"+i,7)
             .addItem("S7-"+i,8)
             .addItem("S8-"+i,9)
             .addItem("S9-"+i,10)
             .addItem("S10-"+i,11)
             .addItem("S11-"+i,12)
             .addItem("S12-"+i,13)
             .addItem("S13-"+i,14)
             .addItem("S14-"+i,15)
             .addItem("S15-"+i,16)
             .activate(synced[i] + 1)
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
    int sync = (int)theEvent.getValue() - 1;
    
    synced[channel] = sync;
    
  }

}
