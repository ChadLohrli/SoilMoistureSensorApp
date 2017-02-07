//Download the following libraries
//Sketch -> Include Library -> Manage Libraries -> Download [ArduinoJson],[OneWire],[DallasTemperature]
//Download the following board module
//http://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include <math.h>

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "sparkfun";

/////////////////////
// Pin Definitions //
/////////////////////
//const int LED_PIN = 12; // Thing's onboard, green LED
const int ANALOG_PIN = A0; // The only analog pin on the Thing
const int TEMP_PIN = 13; // Digital pin to be read

const int numberOfReadings = 25; //number of readings to stabilize ADC readings
const int R2 = 10000; //value of R2 for voltage divider


OneWire oneWire(TEMP_PIN);
DallasTemperature tempSensor(&oneWire);

StaticJsonBuffer<200> jsonBuffer;
JsonObject& root = jsonBuffer.createObject();

WiFiServer server(80);

void setup() 
{
  Serial.begin(115200);
  initHardware();
  setupWiFi();
  server.begin();

  //Start temperature sensor readings
  tempSensor.begin();
}

void loop() 
{
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val = -1; // We'll use 'val' to keep track of both the
                // request type (read/set) and value if set.

  if (req.indexOf("/read") != -1)
    val = -2; // Will print pin reads

  client.flush();

  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: application/json\r\n\r\n";

 if (val == -2)
  { 
  
    //read led
    //digitalWrite(LED_PIN,HIGH);

    int adcValue = 0; //will hold the raw analog value

    //get an average ADC value
    for(int i=0;i<numberOfReadings;i++){

      adcValue += analogRead(ANALOG_PIN);
      delay(25);
    
    }

    int meanAdc = round(adcValue / 25);

  //Serial.println(analogRead(A0));
  //Serial.println(meanTemp);
  
  float voltage = meanAdc * 1.0 / 1023.0; //calculates vout using parameters for ESP8266
  float resistance = R2 * ( 3.28 / voltage - 1.0); // calculates resistance of Sensor through
  //voltage divider equation using input voltage of 3.28v

  root["adcValue"] = meanAdc;
  root["resistance"] = resistance;
  root["voltage"] = voltage;
  root["temperature"] = getTemperature();
  

  }
  else
  {
    s += "Invalid Request.<br>";
  }

  // Send the response to the client
  client.print(s);
  
  // Prints JSON format to client
  root.prettyPrintTo(client);
  delay(100);
  Serial.println("Client disonnected");

  //reading LED off
  //digitalWrite(LED_PIN,LOW);

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}

void setupWiFi()
{
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "ESP8266 Thing " + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}

void initHardware()
{
  Serial.begin(115200);
  pinMode(TEMP_PIN, INPUT);
  //pinMode(LED_PIN, OUTPUT);
  //digitalWrite(LED_PIN, LOW);
}

float getTemperature() {
  float currentTemp;
  tempSensor.requestTemperatures();
  currentTemp = tempSensor.getTempFByIndex(0);
  return currentTemp;
}

