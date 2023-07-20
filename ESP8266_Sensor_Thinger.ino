
#define THINGER_SERIAL_DEBUG
#include <ThingerESP8266.h>

#define USERNAME "Pujaplayschess"
#define DEVICE_ID "ECFABC7C3AC8"
#define DEVICE_CREDENTIAL "my_device_credential"

#define SSID "OPPO A52"
#define SSID_PASSWORD "puja1234"

ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

int Heel, Mid, ForeL, ForeR, walking_stage, fault;

String current_stage;
String Foot_posture;

void setup() {
  // open serial for monitoring
  Serial.begin(115200);


  // add WiFi credentials
  thing.add_wifi(SSID, SSID_PASSWORD);

  thing["Heel"] >> outputValue(Heel);
  thing["Mid"] >> outputValue(Mid);
  thing["ForeL"] >> outputValue(ForeL);
  thing["ForeR"] >> outputValue(ForeR);
  thing["Walking Stage"] >> outputValue(current_stage);
  thing["Walking Style"] >> outputValue(Foot_posture);


  // more details at http://docs.thinger.io/arduino/
}

void loop() {
  thing.handle();

   char buffer[20] = "";
  if(Serial.available() > 0)
  {
      Serial.readBytesUntil('\n', buffer, 20);
//      int result = sscanf(s, "%f,%f,%f,%f,%f,%f,%s", &f1, &f2 ,&f3, &f4, &f5,&f6, d);
      int result = sscanf(buffer, "%d, %d, %d, %d, %d, %d", &Heel, &Mid, &ForeL, &ForeR, &walking_stage, &fault);
//      Serial.println(buffer);

      if(walking_stage == 0)  current_stage = "AIRE";
      else if(walking_stage == 1) current_stage = "HEEL";
      else if(walking_stage == 2) current_stage = "LAND";
      else if(walking_stage == 3) current_stage = "LIFT";

      if(fault == 1)  Foot_posture = "Bad Foot Posture";
      else if(fault == 11) Foot_posture = "Pronated Foot";
      else if(fault == 111) Foot_posture = "Supinated Foot";
      else Foot_posture = "Keep going";

      
      Serial.println(Heel);
      Serial.println(Mid);
      Serial.println(ForeL);
      Serial.println(ForeR);
      Serial.println(current_stage);
  }
}
