/*
  WebArduino
 
 A simple web server to set or reset the arduino-ports 2-9
 and to watch the adc-ports.
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 
 based on the webserver-example from the arduino-gui
 
 Codes by K. Blauel
 Last modified 19 Oct. 2012
 
 Licence: GPL 3.0 or later
 
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

String scanner = "***\n";      // stores the first 1000 bytes of the client-request

void setup() {
  
   for(int i=2; i<=9; i++)  // Define pin 2 - 9 as output
    pinMode(i, OUTPUT);
    
 // Open serial communications and wait for port to open:
  Serial.begin(38400); //! Chance it if you need
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
    Serial.println("\nnew client\n");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        scanner += c;
        
        //Serial.write(c);
        // if you've gotten to the end of the line (received a newline character) and the line is blank, the http request has ended, so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          
          scanner += "***";
          
          controller(scanner);
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<?xml version=\"1.0\"?>");
          client.println("<root>");
                    // add a meta refresh tag, so the browser pulls again every 5 seconds:
          // client.println("<meta http-equiv=\"refresh\" content=\"5\">");
          // output the value of each analog input pin
          
          for (int digitalChannel = 2; digitalChannel <= 9; digitalChannel++) {
            int sensorReading = digitalRead(digitalChannel);
            client.print("\t<data name=\"dout_");
            client.print(digitalChannel);
            client.print("\">");
            client.print(sensorReading);
            client.println("</data>");       
          }
          
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("\t<data name=\"adc_");
            client.print(analogChannel);
            client.print("\">");
            client.print(sensorReading);
            client.println("</data>");       
          }
          client.println("</root>");
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
   
    //Serial.print(scanner);
    //Serial.print("\n"); 
    Serial.print("\nclient disconnected\n");
     
    
    scanner = "";
  }
}

void controller(String string)
{
  
  if(string.indexOf("ardcmd:dw:") >= 0)
  {
    String search = "ardcmd:dw:";
    int startsAt = string.indexOf(search);
    int endsAt = string.indexOf(";", startsAt+1);
    int pin, pinState;
    char part[10];
    String part1, part2;
    
    part1 = string.substring(startsAt+search.length(), startsAt+search.length()+1);
    part2 = string.substring(startsAt+search.length()+2, endsAt);
    
    /*Serial.print("\nardcmd:dw: vorhanden\n");
    
    Serial.print(startsAt); Serial.print("\n"); Serial.print(endsAt);
    
    Serial.print("\n1. Wert:"); Serial.print(part1); Serial.print(":1. Wert");
    
    Serial.print("\n2. Wert:"); Serial.print(part2); Serial.print(":2. Wert\n");
    */
     // match the selected pin
     part1.toCharArray(part,10);
     pin = atoi(part);
      
      // match the selected pinState
     part2.toCharArray(part,10);
     pinState = atoi(part);
     
     //pin = values[0];
     //pinState = values[1];
     
     digitalWrite(pin, pinState);
  }
  
  if(string.indexOf("ardcmd:adc:") >= 0)
  {
    
  }
}
