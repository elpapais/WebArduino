/*
PIN-Webserver

EXAMPLE:
?param=digitalWrite(2,1)
?param=digitalWrite(2,0)

 */  

#include <SPI.h>
#include <Ethernet.h>

// #define DEBUG_SERIAL boolean

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192,168,2, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

String scanner = "";      // stores the first 1000 bytes of the client-request

int stringCounter = 0;  // seeks the position into the scanner-string

int counter = 1;        // counts the allover sum of all requests 

boolean DEBUG_SERIAL = true;

void setup() {
 // Open DEBUG_SERIAL communications and wait for port to open:
  if(DEBUG_SERIAL)
  {
    Serial.begin(38400);
     while (!Serial) {
      ; // wait for DEBUG_SERIAL port to connect. Needed for Leonardo only
    }
  }


  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  
  if(DEBUG_SERIAL)
  {
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());
  }
  
  for(int i=2; i<=9; i++)
    pinMode(i, OUTPUT);

  /*pinMode(2, OUTPUT);  // Set PIN2 to output
  pinMode(3, OUTPUT);  // Set PIN2 to output
  pinMode(4, OUTPUT);  // Set PIN2 to output */
}


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    
    if(DEBUG_SERIAL)
    {
      //Serial.print(counter);
      Serial.println("new client");
    }
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        scanner += c;
        
        stringCounter++;
        
        // DEBUG_SERIAL.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          
          scanner.setCharAt(stringCounter, '\0');
          
          client.println("HTTP/1.1 200 OK");
          client.println("Access-Control-Allow-Origin: *");   
          client.println("Content-Type: application/xml");
          client.println("");
          client.println("<?xml version=\"1.0\" encoding=\"utf-8\" ?>");
          client.println("<root>");
          commandController(scanner, client);
          client.println("</root>");
 
          delay(10);
          client.stop();
            
          // commandController(scanner, client);  // Call the command-controller 
    
          stringCounter = 0;
          scanner = "";
        }
        
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else
          if (c != '\r') {
            // you've gotten a character on the current line
            currentLineIsBlank = false;
        } 
      }
    }
     
    if(DEBUG_SERIAL)
      Serial.println("client disonnected");
    
  }
}

String commandController(String httpString, EthernetClient client)
{

  if(DEBUG_SERIAL)
    Serial.println("1. commandController->parameters="+httpString);

  String parameters = getParameters(httpString);
  
   String retV = "";
  
  if(parameters.indexOf("digitalWrite(") >= 0)
  {
    retV = doDigitalWriteParameters(parameters, client);
    //client.print("digitalWrite()\n");
  }

 return retV;  

}

String getParameters(String string)
{
  int startsAt = string.indexOf("?param=");
  int endsAt;
  
  //if(DEBUG_SERIAL)
    Serial.println("2. getParameters->string="+string);
  
  if(startsAt >=0)
  {
    endsAt = string.indexOf(" ", startsAt);
    
    if(endsAt >=0)
      return string.substring(startsAt, endsAt);
    else
      return "No end-parameters";
  }
  else
    return "No parameters";
}

String doDigitalWriteParameters(String string, EthernetClient client)
{
  int startsAt = string.indexOf("digitalWrite(") + 13;
  int endsAt;
  int kommaAt;
  int pin;
  int pinState;
  
  String ret;
  
  //int *values = matchParameters(string); 
  
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
     
     digitalWrite(pin, pinState);
     
      ret = "ok";
    }
    else
      ret = "No end-parameters";
  }
  else
    ret = "No parameters";
    
   if(DEBUG_SERIAL)
   {
    Serial.print("pin=");
    Serial.println(pin);
  
    Serial.print("pinState=");
    Serial.println(pinState);
   }
  
    return ret;
}
