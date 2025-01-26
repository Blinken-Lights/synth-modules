class ControlPanel extends PApplet {
  //JFrame frame;

  public ControlPanel() {
    super();
    PApplet.runSketch(new String[]{this.getClass().getName()}, this);
  }

  public void settings() {
    size(550, 700);
    smooth();
  }
  public void setup() { 
    windowTitle("Control Panel");
    surface.setLocation(1050, 580);
    
    cp5 = new ControlP5(this);
    
    for(int i = 15; i >= 0; i--){
      
      cp5.addScrollableList("waveTypesDropdown"+i)
         .setId(i)
         .setPosition(0, (i * 20.0))
         .setSize(50, 140)
         .setBarHeight(20)
         .setItemHeight(20)
         .addItem("Buffer"+i,0)
         .addItem("Square"+i,1)
         .addItem("Ramp"+i,2)
         .addItem("InvRamp"+i,3)
         .addItem("Tri"+i,4)
         .addItem("Sine"+i,5)
         .addItem("Exp"+i,6)
         .setColorLabel(color(0))
         .setColorValueLabel(color(0))
         .setColorBackground(color(waveColour[i]))
         .setColorActive(color(waveColour[i]))
         .setType(ScrollableList.DROPDOWN)
         .setValue(waveTypes[i] + 1)
         ;
         
      cp5.addScrollableList("syncDropdown"+i)
         .setId(16 + i)
         .setPosition(50, (i * 20.0))
         .setSize(50, 140)
         .setBarHeight(20)
         .setItemHeight(20)
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
         .setColorLabel(color(0))
         .setColorValueLabel(color(0))
         .setColorBackground(color(waveColour[i]))
         .setColorActive(color(waveColour[i]))
         .setType(ScrollableList.DROPDOWN)
         .setValue(synced[i] + 1)
         ;
         
      cp5.addScrollableList("ampModDropdown"+i)
         .setId(32 + i)
         .setPosition(100, (i * 20.0))
         .setSize(50, 140)
         .setBarHeight(20)
         .setItemHeight(20)
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
         .setColorLabel(color(0))
         .setColorValueLabel(color(0))
         .setColorBackground(color(waveColour[i]))
         .setColorActive(color(waveColour[i]))
         .setType(ScrollableList.DROPDOWN)
         .setValue(amplitudeModulation[i] + 1)
         ;
         
      cp5.addScrollableList("freqModDropdown"+i)
         .setId(48 + i)
         .setPosition(150, (i * 20.0))
         .setSize(50, 140)
         .setBarHeight(20)
         .setItemHeight(20)
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
         .setColorLabel(color(0))
         .setColorValueLabel(color(0))
         .setColorBackground(color(waveColour[i]))
         .setColorActive(color(waveColour[i]))
         .setType(ScrollableList.DROPDOWN)
         .setValue(frequencyModulation[i] + 1)
         ;
         
      cp5.addScrollableList("sampleHoldDropdown"+i)
         .setId(64 + i)
         .setPosition(200, (i * 20.0))
         .setSize(50, 140)
         .setBarHeight(20)
         .setItemHeight(20)
         .addItem("SHNone"+i,0)
         .addItem("SH0-"+i,1)
         .addItem("SH1-"+i,2)
         .addItem("SH2-"+i,3)
         .addItem("SH3-"+i,4)
         .addItem("SH4-"+i,5)
         .addItem("SH5-"+i,6)
         .addItem("SH6-"+i,7)
         .addItem("SH7-"+i,8)
         .addItem("SH8-"+i,9)
         .addItem("SH9-"+i,10)
         .addItem("SH10-"+i,11)
         .addItem("SH11-"+i,12)
         .addItem("SH12-"+i,13)
         .addItem("SH13-"+i,14)
         .addItem("SH14-"+i,15)
         .addItem("SH15-"+i,16)
         .setColorLabel(color(0))
         .setColorValueLabel(color(0))
         .setColorBackground(color(waveColour[i]))
         .setColorActive(color(waveColour[i]))
         .setType(ScrollableList.DROPDOWN)
         .setValue(sampleAndHoldTrigger[i] + 1)
         ;
         
      cp5.addScrollableList("quantisationDropdown"+i)
         .setId(80 + i)
         .setPosition(250, (i * 20.0))
         .setSize(70, 400)
         .setBarHeight(20)
         .setItemHeight(20)
         .addItem("QNone-"+i,0)
         .addItem("Chromatic-"+i,1)
         .addItem("Dorian-"+i,2)
         .addItem("Phrygian-"+i,3)
         .addItem("Lydian-"+i,4)
         .addItem("Mixolydian-"+i,5)
         .addItem("Aeolian-"+i,6)
         .addItem("Locrian-"+i,7)
         .addItem("LydianDom-"+i,8)
         .addItem("SupLocrian-"+i,9)
         .addItem("Hirajoshi-"+i,10)
         .addItem("MinPent-"+i,11)
         .addItem("MajPent-"+i,12)
         .addItem("MinBlues-"+i,13)
         .addItem("MajBlues-"+i,14)
         .addItem("WholeHalf-"+i,15)
         .addItem("HalfWhole-"+i,16)
         .setColorLabel(color(0))
         .setColorValueLabel(color(0))
         .setColorBackground(color(waveColour[i]))
         .setColorActive(color(waveColour[i]))
         .setType(ScrollableList.DROPDOWN)
         .setValue(quantise[i] + 1)
         ;
             
    }
  }

  public void draw() {
    background(0);
  }
  
  void controlEvent(ControlEvent theEvent) {
    int controllerType = floor(theEvent.getId() / 16);
    int channel = theEvent.getId() % 16;
    
    if(controllerType == 0){
      int waveType = (int)theEvent.getValue() - 1;
      waveTypes[channel] = waveType;
    }else if(controllerType == 1){
      int syncToChannel = (int)theEvent.getValue() - 1;
      synced[channel] = syncToChannel;
    }else if(controllerType == 2){
      int ampModByChannel = (int)theEvent.getValue() - 1;
      amplitudeModulation[channel] = ampModByChannel;
    }else if(controllerType == 3){
      int freqModByChannel = (int)theEvent.getValue() - 1;
      frequencyModulation[channel] = freqModByChannel;
    }else if(controllerType == 4){
      int sampleAndHoldByChannel = (int)theEvent.getValue() - 1;
      sampleAndHoldTrigger[channel] = sampleAndHoldByChannel;
    }else if(controllerType == 5){
      int quantisation = (int)theEvent.getValue() - 1;
      quantise[channel] = quantisation;
    }
    
  }

}
