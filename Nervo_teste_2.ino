
#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>
//inclusion of the libs

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


//define the pins of output and input
#define BUTTON 2
#define END 6
#define MID 7
#define SPEAKER 8

//Config the display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     4
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//define the notes for the tone func
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_FS5 740
#define NOTE_GS5 831


Encoder myEnc(5, 4);
//declaretion of the variables
bool play = false;

uint8_t counter = 6;
int8_t option = 0;

int melody[] = {NOTE_B0  , NOTE_C1  , NOTE_CS1 , NOTE_D1  , NOTE_DS1 , NOTE_E1  , NOTE_F1  , NOTE_FS1 , NOTE_G1  ,
                NOTE_GS1
               };

int noteDurations[] = {
  1, 2, 4, 8, 1, 2, 4, 8
};

//Pin change interrupt routine
ISR(PCINT2_vect) {
  
  //enable the interruptions 
  sei();

  //disable the interruptions in the 
  //ports of the end and the midle
  PCMSK2 =(0<<END)|(0<<MID);
  
  if (!digitalRead(MID)){
    lose();   
    } 
    
  else if(!digitalRead(END)) {
    win();
  }

}

//TIMER interrupt routine
ISR(TIMER1_COMPA_vect) {

  //enable the interruptions 
  sei();

  //Config the display
  display.clearDisplay();
  display.setCursor(32, 12);
  display.setTextSize(6);
  display.drawRoundRect(0, 0, display.width(), display.height(), display.height() / 8, SSD1306_WHITE);
  
  //write 0 if the counter were less than 10
  if (counter < 10) display.print(0);

  //Write the counter
  display.print(counter);

  //if the tima were up to go the loss routine
  if (counter == 0)  lose();

  //decrease the counter
  counter--;

  //Show the count on the display
  display.display();

}


void lose() {

  //Disable the timmer and the song
  TIMSK1 = 0B00000000;
  play = false;
  noTone(SPEAKER);

  //Write the loss mesage 
  display.setCursor(0, 0);
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(48, 5);
  display.println("YOU");
  display.setCursor(44, 25);
  display.println("LOSE");
  display.setCursor(48, 45);
  display.println("!!!");
  display.drawRoundRect(0, 0, display.width(), display.height(), display.height() / 8, SSD1306_WHITE);
  display.display();

  //Song of Loss
  tone(SPEAKER  , NOTE_FS5, 1000);
  
  //wait the press of the button
  while(digitalRead(BUTTON)) ;
  delay(100);//anti bounce

  //Go to menu routine to restrat the game
  menu();
  while(digitalRead(BUTTON));
  delay(500);
}

void win() {
  //Disable the timmer and the song
  TIMSK1 = 0B00000000;
  play = false;
  noTone(SPEAKER);

  //Write the win mesage 
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(48, 5);
  display.println("YOU");
  display.setCursor(48, 25);
  display.println("WIN");
  display.setCursor(48, 45);
  display.println("!!!");
  display.drawRoundRect(0, 0, display.width(), display.height(), display.height() / 8, SSD1306_WHITE);
  display.display();

  //Song of victory
  tone(SPEAKER  , NOTE_GS5, 125);
  delay(100);
  tone(SPEAKER  , NOTE_FS5, 250);
  delay(1000);

  //wait the press of the button
  while(digitalRead(BUTTON));
  delay(100);//anti bounce
  
  //Go to menu routine to restrat the game
  menu();
  while(digitalRead(BUTTON));
  delay(500);
}

void menu() {
  display.clearDisplay();

  //delay(500);//To avoid bounce
  
  //option++;

  //if (option ==4) option = 1;

  
  
  

  //Config the display
  display.drawRoundRect(0, 0, display.width(), display.height(), display.height() / 8, SSD1306_WHITE);
  display.setCursor(10, 5);

  display.setTextSize(1);
  display.print(" Select the speed:");
  display.setCursor(0, 38);
  

  display.print("\n\n     Hold to go");
  display.setTextSize(3);

  
  if (option == 0 && !play) {
    display.setCursor(28, 24);
    display.println("Slow");
    display.display();
    counter = 80;
  }

  if (option == 2 && !play) {
    display.setCursor(12, 24);
    display.println("Medium");
    display.display();
    counter = 60;
  }

  if (option >= 4 && !play) {
    display.setCursor(28, 24);
    display.println("Fast");
    display.display();
    counter = 40;
    
  }



}

void setup() {

  //Pin configuration
  //pinMode(SPEAKER,OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(END, INPUT_PULLUP);
  pinMode(MID, INPUT_PULLUP);

  //Pin interuptions configuration
  PCICR |= (1<<PCIE2);
  PCMSK2 =(0<<END)|(0<<MID);

  //Timmer configuration
  TCCR1A = 0B00000000;
  TCCR1B = 0B00001101;
  OCR1A = 15625;

  //Display configuration
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.clearDisplay();
  Serial.begin(9600);
  
  //Function to dwan the menu into the display
  menu();

}

void loop() {

  if (!digitalRead(BUTTON)) {
    display.clearDisplay();
    tone(SPEAKER  , NOTE_FS5, 1000);

    display.setTextSize(3);
    display.setCursor(20, 20);
    display.println("Begin");
    display.display();
    while (!digitalRead(BUTTON));
    TIMSK1 = 0B00000010;
    PCMSK2 =(1<<END)|(1<<MID);

    play = true;

    //option--;
    //option--;

  }

  if (play) {
    for (int thisNote = 0; thisNote < 8; thisNote++) {

      int noteDuration = 1000 / noteDurations[thisNote];

      if(!play) {
        noTone(SPEAKER);
        break;
        
        }
      tone(SPEAKER, melody[thisNote], (noteDuration));
      Serial.println(play);
      int pauseBetweenNotes = noteDuration * 1.30;

      delay(pauseBetweenNotes);

      noTone(SPEAKER);

    }
  }
  
  if(option != myEnc.read()){
    option = myEnc.read();
    
    if (option>4) {
      
      option=0;
      myEnc.write(0); 
    }
    if (option<0) {
      option=4;
      myEnc.write(4);
    }
    menu();
  }     

}
