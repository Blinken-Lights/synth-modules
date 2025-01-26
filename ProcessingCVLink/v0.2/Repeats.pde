class Repeats extends PApplet {
  //JFrame frame;
  
  int[] firstEvents = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  
  public Repeats() {
    super();
    PApplet.runSketch(new String[]{this.getClass().getName()}, this);
  }

  public void settings() {
    size(550, 320);
    smooth();
  }
  public void setup() { 
    windowTitle("Repeats");
    surface.setLocation(0, 580);
    
    cp5 = new ControlP5(this);
    
    for(int i = 0; i < 16; i++){
      
      cp5.addSlider("repeatsSlider"+i)
       .setId(i)
       .setColorForeground(color(waveColour[i]))
       .setColorActive(color(waveColour[i]))
       .setPosition(0, (i * 20.0))
       .setSize(550,20)
       .setRange(-1,32)
       .setValue(repeats[i])
       ;
             
    }
  }

  public void draw() {
    background(0);
  }
  
  void controlEvent(ControlEvent theEvent) {
    int channel = theEvent.getId();
    int repeat = round(theEvent.getValue());
    
    if(firstEvents[channel] < 3){
      firstEvents[channel]++;
      return;
    }
    
    repeats[channel] = repeat;
    
  }
  
  public void repeatsSlider0(int value){
    
  }
  public void repeatsSlider1(int value){
    
  }
  public void repeatsSlider2(int value){
    
  }
  public void repeatsSlider3(int value){
    
  }
  public void repeatsSlider4(int value){
   
  }
  public void repeatsSlider5(int value){
    
  }
  public void repeatsSlider6(int value){
    
  }
  public void repeatsSlider7(int value){
    
  }
  public void repeatsSlider8(int value){
    
  }
  public void repeatsSlider9(int value){
    
  }
  public void repeatsSlider10(int value){
    
  }
  public void repeatsSlider11(int value){
    
  }
  public void repeatsSlider12(int value){
    
  }
  public void repeatsSlider13(int value){
    
  }
  public void repeatsSlider14(int value){
    
  }
  public void repeatsSlider15(int value){
    
  }

}
