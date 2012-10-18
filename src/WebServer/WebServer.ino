/*
  WebArduino
 
 A simple web server to manipulate the OUT-Pins by using a webbrowser. Also
 it's possible to read the adc-values
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 
 based on the werserver-example of the arduino-GUI
 
 
 modified 18 Oct 2012
 coded by K. Blauel
 
 */

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1, 177);
// IPAddress ip(192,168,1, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

String scanner = "";      // stores the first 1000 bytes of the client-request

void setup() {
  
   for(int i=2; i<=9; i++)  // Define pin 2 - 9 as output
    pinMode(i, OUTPUT);
    
 // Open serial communications and wait for port to open:
  Serial.begin(38400);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        scanner += c;
        
        //Serial.write(c);
        // if you've gotten to the end of the line (received a newline character) and the line is blank, the http request has ended, so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          
          controller(scanner);
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
                    // add a meta refresh tag, so the browser pulls again every 5 seconds:
          // client.println("<meta http-equiv=\"refresh\" content=\"5\">");
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("\t<data name=\"adc_");
            client.print(analogChannel);
            client.print("\">");
            client.print(sensorReading);
            client.println("</data>");       
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(10);
    // close the connection:
    client.stop();
   
    Serial.print(scanner);
    Serial.print("\n"); 
    //controller(scanner);
    Serial.print("client disonnected\n");
     
    
    scanner = "";
  }
}

void controller(String string)
{
  String parameters = getParameters(string, "?param=", " ");
  
  if(parameters.indexOf("digitalWrite(") >= 0)
    doDigitalWrite(parameters);
    
  if(parameters.indexOf("analogRead(") >= 0)
    Serial.println("analogRead: "+doAnalogRead(parameters));
    
}

String getParameters(String string, String paramName, String endParam)
{
  int startsAt = string.indexOf(paramName);
  int endsAt;
  
  String retV = "null";
  
  //Serial.println("2. getParameters->string="+string);
  
  if(startsAt >=0)
  {
    endsAt = string.indexOf(endParam, startsAt) + 1;
    
     // msg1(startsAt, endsAt);
     
    if(endsAt >=0)
      retV = string.substring(startsAt, endsAt);
    else
      retV = "";
  }
 
  return retV;
}

int *getParameterValues(String string, String paramName)
{
  
  //Serial.print("3. getParameterValues->string_"+string);
  //Serial.print("_ paramName->"+paramName);
  int* pointer;
  int startsAt = string.indexOf(paramName) + paramName.length();
  int endsAt;
  int kommaAt;
  int pin=-1;
  int pinState=-1;
  
  String ret;

  int values[2]; //  = matchParameters(string); 
  
  pointer = values;
  
  if(startsAt >=0)
  {
    endsAt = string.indexOf(")", startsAt);

    if(endsAt >=0)
    {
      kommaAt = string.indexOf(",");
           
      char part[10];
     
      // match the selected pin
      string.substring(startsAt, kommaAt).toCharArray(part,10);
      pin = atoi(part);
      
      // match the selected pinState
      string.substring(kommaAt+1, endsAt).toCharArray(part,10);
      pinState = atoi(part);
      
     //pin = values[0];
     //pinState = values[1];
     
     values[0] = pin;
     values[1] = pinState;
    }
  }
  
  return pointer;
}

String doDigitalWrite(String string)
{
  //Serial.println("1. digitalWrite: "+string);  // "digitalWrite("
  
  int *pinData;
  
  String retV = "null";
  String parameters = getParameters(string, "digitalWrite(", ")");

  //Serial.print("\n2. digitalWrite: "+parameters); Serial.print(parameters);
  
  pinData = getParameterValues(parameters, "digitalWrite(");
  
  if(pinData[0] >=0 && pinData[1] >= 0)
    digitalWrite((int)pinData[0], (int)pinData[1]);
    //digitalWrite(2, 1);
    
  /*Serial.print("\npin=");
  Serial.print(pinData[0]);
  Serial.print("\npinState=");
  Serial.print(pinData[1]);
  
*/
  
}

String doAnalogRead(String string)
{
  String retV = "null";
  
  
}

/*void msg1(int startsAt, int endsAt)
{
    Serial.print("\nstartsAt=");
    Serial.print(startsAt);
    Serial.print("\nendsAt=");
    Serial.print(endsAt);
   // Serial.print("\,kommaAt=");
   // Serial.println(kommaAt);
 }

void msg2(int pin, int pinState)
{
  Serial.print("\npin = ");
  Serial.print(pin);
  Serial.print("\npinState = ");
  Serial.print(pinState);
  Serial.println("");
  

}*/