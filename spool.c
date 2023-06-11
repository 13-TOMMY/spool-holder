#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "dht.h"
#include "Arduino.h"
#include "HX711.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

const int WEIGHT_DT_LEFT = 2;
const int WEIGHT_SCK_LEFT = 3;
const int WEIGHT_DT_MIDDLE = 4;
const int WEIGHT_SCK_MIDDLE = 5;
const int WEIGHT_DT_RIGHT = 6;
const int WEIGHT_SCK_RIGHT = 7;
const int TEMP_PIN = 8;
const int BACK_BUTTON_PIN = 10;
const int CONFIRM_BUTTON_PIN = 9;
const int FORWARD_BUTTON_PIN = 11;
const int TEST_LED = 13;
int DISPLAY_TIMEOUT = 0;
bool DISPLAY_ON = 1;
int TIMER_1 = 0;
bool TIMER_STATE = 0;
bool MENU_1 = 0;
bool MENU_2 = 0;
bool MENU_3 = 0;

// Button state variables
int backButtonState = 0;
int confirmButtonState = 0;
int forwardButtonState = 0;

dht DHT;
HX711 scaleL;
HX711 scaleM;
HX711 scaleR;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void(* resetFunc) (void) = 0;

void setup() {
  // Initialize the display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.display();

  //Pinout
  pinMode(BACK_BUTTON_PIN, INPUT);
  pinMode(CONFIRM_BUTTON_PIN, INPUT);
  pinMode(FORWARD_BUTTON_PIN, INPUT);
  pinMode(TEST_LED, OUTPUT);
  digitalWrite(BACK_BUTTON_PIN, HIGH);
  digitalWrite(CONFIRM_BUTTON_PIN, HIGH);
  digitalWrite(FORWARD_BUTTON_PIN, HIGH);

  Serial.begin(9600);
  Serial.println("Initializing the scale");
  scaleL.begin(WEIGHT_DT_LEFT, WEIGHT_SCK_LEFT);
  scaleM.begin(WEIGHT_DT_MIDDLE, WEIGHT_SCK_MIDDLE);
  scaleR.begin(WEIGHT_DT_RIGHT, WEIGHT_SCK_RIGHT);
         
  scaleL.set_scale(-475.005);
  scaleL.tare();
  scaleM.set_scale(-460.770);
  scaleM.tare();
  scaleR.set_scale(-441.333);
  scaleR.tare();
  Serial.println("Initialisation finished");
  
  //timer_1
  TCCR0A=(1<<WGM01);   
  OCR0A=0xF9;
  TIMSK0|=(1<<OCIE0A);
  sei();
  TCCR0B|=(1<<CS01);
  TCCR0B|=(1<<CS00);

}
void loop() {

  confirmButtonState = digitalRead(CONFIRM_BUTTON_PIN);
  backButtonState = digitalRead(BACK_BUTTON_PIN);
  forwardButtonState = digitalRead(FORWARD_BUTTON_PIN);
  //displayMenu1();
  //displayMenu2();
  //displayMenu3();
  if (confirmButtonState == HIGH) {  
    Serial.println("confirm button");
    DISPLAY_TIMEOUT = 0;
    delay(200);
    if(confirmButtonState == HIGH && DISPLAY_ON == 1){
      displayMenu1();
        if(confirmButtonState == HIGH && MENU_1 == 1){
          scaleL.set_scale(-475.005);
          scaleL.tare();
          MENU_1 = 0;
          Serial.println("scale L tare");
        }
      MENU_1 = 1;
    }
  }

  if (backButtonState == HIGH) {    
    Serial.println("back button");
    DISPLAY_TIMEOUT = 0;
    resetFunc(); 
    delay(200);
  }
  
  if (forwardButtonState == HIGH) {    
    Serial.println("forward button");
    DISPLAY_TIMEOUT = 0;
      //scaleL.set_scale(-475.005);
      //scaleL.tare();
      //scaleM.set_scale(-460.770);
      //scaleM.tare();
      //scaleR.set_scale(-441.333);
      //scaleR.tare();
    delay(200);
    //if(){
      
    //}
  }
    

  float weightL = scaleL.get_units();
  float weightM = scaleM.get_units();
  float weightR = scaleR.get_units();
  int readData = DHT.read22(TEMP_PIN);
  float temperature = DHT.temperature;
  float humidity = DHT.humidity;

  if(DISPLAY_TIMEOUT < 100){
    wakeUpDisplay();
    if(DISPLAY_TIMEOUT < 1 ){
      displayWeightL(0);
      displayWeightM(0);
      displayWeightR(0);
      displayTemperatureAndHumidity();
    }
    if(weightL >= 10 ){
      displayWeightL(weightL);
    } else {
      displayWeightL(0);
    }

    if(weightM >= 10){
      displayWeightM(weightM);
    } else {
      displayWeightM(0);
    }

    if(weightR >= 10){
      displayWeightR(weightR);
    } else {
      displayWeightR(0);
    }

    displayTemperatureAndHumidity();

    if(TIMER_1>=1000){
    TIMER_STATE=!TIMER_STATE;
    TIMER_1=0;
    }
    if(TIMER_STATE == 1){
    DISPLAY_TIMEOUT = DISPLAY_TIMEOUT + 1;
    }
    DISPLAY_ON = 1;

  } else {
    turnOffDisplay();
    DISPLAY_ON = 0;
  }
}

void displayTemperatureAndHumidity() {
  int textHeight = 8;
  int gap = 0;
  int bottomY = SCREEN_HEIGHT - textHeight;
  display.fillRect(0, bottomY - textHeight - gap, SCREEN_WIDTH, textHeight * 2, SSD1306_BLACK);
  display.setCursor(0, bottomY - gap);
  display.print("T:");
  display.print(DHT.temperature);
  display.print("C | H: ");
  display.print(DHT.humidity);
  display.print("%");
  display.display();
}

void displayWeightL(float weightL) {
  int textHeight = 8;
  int gap = 54;
  int topY = SCREEN_HEIGHT - textHeight;
  display.fillRect(0, topY - textHeight - gap, SCREEN_WIDTH - 24, textHeight * 2, SSD1306_BLACK);
  display.setCursor(0, topY - gap);
  display.print("Weight L: ");
  display.print(weightL);
  display.print(" g");
  display.display();
}

void displayWeightM(float weightM) {
  int textHeight = 8;
  int gap = 38;
  int topY = SCREEN_HEIGHT - textHeight;
  display.fillRect(0, topY - textHeight - gap, SCREEN_WIDTH - 24, textHeight * 2, SSD1306_BLACK);
  display.setCursor(0, topY - gap);
  display.print("Weight M: ");
  display.print(weightM);
  display.print(" g");
  display.display();

}
void displayWeightR(float weightR) {
  int textHeight = 8;
  int gap = 20;
  int topY = SCREEN_HEIGHT - textHeight;
  display.fillRect(0, topY - textHeight - gap, SCREEN_WIDTH - 24, textHeight * 2, SSD1306_BLACK);
  display.setCursor(0, topY - gap);
  display.print("Weight R: ");
  display.print(weightR);
  display.print(" g");
  display.display();
}

void wakeUpDisplay() {
  display.ssd1306_command(SSD1306_DISPLAYON);
}

void turnOffDisplay() {
  display.clearDisplay();
  display.display();
  display.ssd1306_command(SSD1306_DISPLAYOFF);
}

void displayMenu1() {
  int textHeight = 8;
  int gap = 54;
  int rightX = SCREEN_WIDTH - 24;
  int topY = SCREEN_HEIGHT - textHeight;
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(rightX, topY - gap);
  display.print("<<<");
  display.display();
}

void displayMenu2() {
  int textHeight = 8;
  int gap = 38;
  int rightX = SCREEN_WIDTH - 24;
  int topY = SCREEN_HEIGHT - textHeight;
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(rightX, topY - gap);
  display.print("<<<");
  display.display();
}

void displayMenu3() {
  int textHeight = 8;
  int gap = 20;
  int rightX = SCREEN_WIDTH - 24;
  int topY = SCREEN_HEIGHT - textHeight;
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(rightX, topY - gap);
  display.print("<<<");
  display.display();
}

//Timer
ISR(TIMER0_COMPA_vect){ 
  TIMER_1++;
}