class Quantisation extends PApplet {
  //JFrame frame;

  public Quantisation() {
    super();
    PApplet.runSketch(new String[]{this.getClass().getName()}, this);
  }

  public void settings() {
    size(1350, 320);
    smooth();
  }
  public void setup() { 
    windowTitle("Quantisation");
    surface.setLocation(0, 650);
    
    cp5 = new ControlP5(this);
    
    for(int i = 0; i < 16; i++){
      
      cp5.addRadioButton("quantisationButton"+i)
             .setId(i)
             .setPosition(0, (i * 20.0))
             .setSize(20,20)
             .setColorLabel(color(waveColour[i]))
             .setColorForeground(color(120))
             .setColorActive(color(waveColour[i]))
             .setItemsPerRow(17)
             .setSpacingColumn(60)
             .addItem("QNone"+i,0)
             .addItem("Chromatic"+i,1)
             .addItem("Dorian"+i,2)
             .addItem("Phrygian"+i,3)
             .addItem("Lydian"+i,4)
             .addItem("Mixolydian"+i,5)
             .addItem("Aeolian"+i,6)
             .addItem("Locrian"+i,7)
             .addItem("LydianDom"+i,8)
             .addItem("SupLocrian"+i,9)
             .addItem("Hirajoshi"+i,10)
             .addItem("MinPent"+i,11)
             .addItem("MajPent"+i,12)
             .addItem("MinBlues"+i,13)
             .addItem("MajBlues"+i,14)
             .addItem("WholeHalf"+i,15)
             .addItem("HalfWhole"+i,16)
             .activate(quantise[i] + 1)
             ;
             
    }
  }

  public void draw() {
    background(0);
  }
  
  void controlEvent(ControlEvent theEvent) {
    int channel = theEvent.getId();
    int quantisation = (int)theEvent.getValue() - 1;
    
    quantise[channel] = quantisation;
    
  }

}
