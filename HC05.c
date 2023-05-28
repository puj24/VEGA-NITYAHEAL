/*
  @file Bluetooth_HC05.ino
  @brief contains routines for UART HC05 interface
  @detail Includes software functions declarations to initialize,configure, write and read HC05 over UARt interface

   Reference arduino code: https://electropeak.com/learn/
   Refrence aries board: https://vegaprocessors.in/blog/hc-05-bluetooth-module-with-aries-v2-board/
   Vega Arduino app apk: https://drive.google.com/file/d/1I8Gzlu3QLy9f35Pzw_uch31uhC18UtSt/view?usp=sharing
   
   **HC05 bluetooth sensor**
   Connections:
   HC05     Aries Board
   VCC      -   3.3V
   GND      -   GND
   RXD      -   TX1
   TXD      -   RX1
   For connecting to port 2 (RX2 and TX2) of aries board use the function UARTClass MyBlue(2) instead of UARTClass MyBlue(1);
*/

#include <UARTClass.h>

UARTClass MyBlue(1); // RX | TX

int flag = 0;
int LED = 22;  // Green LED

int send_string(char * str)

{

  while (*str != '\0') {

    MyBlue.write(*str);

    if (*str == '\n') //newline character

      MyBlue.write('\r');

    str++;

  }

  return 0;

}


void receive_string(char * str)

{
  int data;
  char char_data;

  
  while (1) {

    do{
      data = MyBlue.read();
    }while(data == -1);

  char_data = (char)data;
    
    *str = char_data;

    str++;

    if (char_data == '\n') {
      *str = '\r';

      break;

    }

  }

}

void setup()
{
  Serial.begin(115200);
  MyBlue.begin(9600);
  pinMode(LED, OUTPUT);
  Serial.println("Ready to connect\nDefualt password is 1234 or 000");
}
void loop()
{

  static char str[100] = {0,};

  memset(str, 0, sizeof(str));

  receive_string((char*)str);

  Serial.println(str);

  if (strncmp(str, "ON", 2) == 0)

  {

    digitalWrite(LED, LOW);

    send_string((char*)"LED ON\n");

  }

  else

  {

    send_string((char*)"LED OFF\n");

    digitalWrite(LED, HIGH);

  }


}