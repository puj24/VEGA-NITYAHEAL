/*
  @file voltage_example.ino
  @brief Read volatge value 
  @detail This demo display the volatge (in volts) on serial monitor

   Aries board PINOUT: https://vegaprocessors.in/files/PINOUT_ARIES%20V2.0_.pdf
   
   *** Voltage Example ***
   Connections:
   Aries       Aries Board
   A0      -   3.3V
*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <esp8266.h>

ESP8266Class esp8266(1);
//_______________________________________UART_____________________________________________________________________

char str_Heel[25];
char str_Mid[25];
char str_ForeL[25];
char str_ForeR[25];

//___________________________________________OLED_________________________________________________________________

TwoWire Wire(0);
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)

#define OLED_RESET     -1 // Reset pin # not required
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };

//________________________________________FSR_______________________________________

#define PIN0 A0  // Reading value from Analog pin 0
#define PIN1 A1
#define PIN2 A2
#define PIN3 A3
#define THRESHOLD 500

//random values as of now
int MAX[5] = {2000, 2000, 2000, 2000};
int MIN[5] = {THRESHOLD, THRESHOLD, THRESHOLD, THRESHOLD};
 
int  Sensor1, Sensor2, Sensor3, Sensor4 ; 
int prev_stage = 0;
int current_stage;
float resolution = 1;
int fault = 0;
int fault_p = 0;
int Vibrational_Pin = 0;

//________________________________________________SETUP_____________________________________________________________
void setup()
{
  Serial.begin(115200);
  esp8266.begin(115200); //for UART

   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  pinMode(Vibrational_Pin, OUTPUT);
}


//______________________________________________POSTURE DETECTION MAIN__________________________________________________________
void loop() {
  // put your main code here, to run repeatedly:
  Sensor1 = resolution * (float)analogRead(PIN0);
  Sensor2 = resolution * (float)analogRead(PIN1);
  Sensor3 = resolution * (float)analogRead(PIN2);
  Sensor4 = resolution * (float)analogRead(PIN3);

  if(Sensor1 > 3000) Sensor1 = 0;
  if(Sensor2 > 3000) Sensor2 = 0;
  if(Sensor3 > 3000) Sensor3 = 0;
  if(Sensor4 > 3000) Sensor4 = 0;
  
  Serial.print("Heel:");
  Serial.print(Sensor1);
  Serial.print(",");
  Serial.print("Mid:");
  Serial.print(Sensor2);
  Serial.print(",");
  Serial.print("ForeL:");
  Serial.print(Sensor3);
  Serial.print(",");
  Serial.print("ForeR:");
  Serial.print(Sensor4);
  Serial.print(",");
  Serial.print("THRESHOLD:");
  Serial.print(THRESHOLD);
 
  //str =String("coming from arduino: ")+String("Heel= ")+String(Sensor1)+String("Mid= ")+String(Sensor2)+ String("ForeL= ")+String(Sensor3)+String("ForeR= ")+String(Sensor4);
  
//  sprintf(data,"{\"value\": %d}",getSensorData());
  sprintf(str_Heel, "%d%s", Sensor1, ",");
  sprintf(str_Mid, "%d%s", Sensor2, ",");
  sprintf(str_ForeL, "%d%s", Sensor3, ",");
  sprintf(str_ForeR, "%d%s", Sensor4, ",");
  esp8266.print(str_Heel);
  esp8266.print(str_Mid);
  esp8266.print(str_ForeL);
  esp8266.print(str_ForeR);
//  esp8266.println("puja");

  
  detectstage(prev_stage, &current_stage, Sensor1, Sensor2, Sensor3, Sensor4);
  prev_stage = current_stage;

  Serial.print(",");
  Serial.print("CURRENTSTAGE:");
  esp8266.print(current_stage);
  Serial.print(current_stage*500);
  
  if(current_stage != 0)
  {
    //check for bad foot posture
    if(current_stage == 1)  //HEEL
    {
      if(Sensor1 > MAX[0] && Sensor1 < MIN[0])
      {
        fault = 1;
      }
    }
    else if(current_stage == 2) //LAND
    {
      if(Sensor2 > MAX[1] && Sensor2 < MIN[1])
      {
        fault = 1;
      }
      if(Sensor1 < THRESHOLD)
      {
        if(Sensor1 < Sensor2)  //Pronated foot
        {
          fault = 11;
        }
      }
    }
    else if(current_stage == 3) //LIFT
    {
      if(Sensor3 > MAX[2] && Sensor3 < MIN[2])
      {
        fault = 1;
      }
      if(Sensor4 > MAX[3] && Sensor4 < MIN[3])
      {
        fault = 1;
      }
      if(Sensor3 > Sensor4) //Supinated foot
      {
        fault = 111;
      }
    }
  }
  if(fault >= 1)
  {
    fault_p = 1;
  }
  else
  {
    fault_p = 0;
  }

  //Sending Fault value detected
  esp8266.println(fault);
  
  Serial.print(",");
  Serial.print("Fault:");
  Serial.println(fault_p*2000);
  if(fault >= 1)
  {
    if(fault == 11)
    {
      Draw_pronated();
    }
    else if (fault == 111)
    {
      Draw_supinated();
    }
    digitalWrite(Vibrational_Pin, HIGH);
    delay(1000);
    digitalWrite(Vibrational_Pin, LOW);
    fault = 0;
  }
  
  delay(1);
}


//________________________________________________STAGE DETECTOR___________________________________________________

void detectstage(int prev_stage, int* current_stage, float Sensor1, float Sensor2, float Sensor3, float Sensor4)
{
    if(Sensor1 < THRESHOLD && Sensor2 < THRESHOLD && Sensor3 < THRESHOLD && Sensor4 < THRESHOLD)
    {
      *current_stage = 0;
    }
    else if(Sensor1 > THRESHOLD && Sensor2 < THRESHOLD && Sensor3 < THRESHOLD && Sensor4 < THRESHOLD)
    {
      *current_stage = 1;
    }

    else if(Sensor1 > THRESHOLD && Sensor2 > THRESHOLD && Sensor3 > THRESHOLD && Sensor4 > THRESHOLD)
    {
      *current_stage = 2;
    }
    else if(Sensor1 < THRESHOLD && Sensor2 < THRESHOLD && Sensor3 > THRESHOLD && Sensor4 > THRESHOLD)
    {
      *current_stage = 3;
    }
}


//___________________________________OLED draw foot styles_____________________________________________

void Draw_supinated(void) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Supinated Foot!"));

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.println(3.141592);

  display.display();
  delay(2000);
  display.clearDisplay();
}

String int_to_string(int Sensor)
{
  String str;
  str = Sensor/10 + '0';
  return str;
}

void Draw_pronated(void) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Pronated Foot!"));

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.println(3.141592);

  display.display();
  delay(2000);
  display.clearDisplay();
}
