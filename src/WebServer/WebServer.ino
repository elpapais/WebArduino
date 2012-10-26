/*
  WebArduino: Ein Projekt der Werner-von-Siemens-Schule Koelnue
 
  Per Webserver werden HTTP-Request ausgewertet, um die Zustaende
  von Ausgang-Bits zu setzen und die Werte des AD-Wandlers
  auszulesen.
  Die Hardware baut auf dem Arduino (mit ATmega328) mit
  Ethernet-Shield auf. 
 
 Anschluesse:
 * Das Ethernet-Shield belegt die Pins 10, 11, 12 und 13
 * Pin 0 und 1 sind fuer UART reserviert
 * Die Analogeingaenge belegen pins A0 bis A5 (optional)
 
 Das Programm basiert auf dem Webserver-Beispiel der Arduino-Oberflaeche
 
 (c) 2012 by K. Blauel
 Letzte Aenderung: 26 Okt. 2012
 
 Lizenz: GPL 3.0 oder hoeher.
 Dieses Programm ist freie Software. Sie koennen es unter den Bedingungen
 der GNU General Public License, wie von der Free Software Foundation
 veroeffentlicht, weitergeben und/oder modifizieren, entweder gemaeß
 Version 3 der Lizenz oder (nach Ihrer Option) jeder spaeteren Version.

 Die Veroeffentlichung dieses Programms erfolgt in der Hoffnung,
 daß es Ihnen von Nutzen sein wird, aber OHNE IRGENDEINE GARANTIE,
 sogar ohne die implizite Garantie der MARKTREIFE oder der VERWENDBARKEIT
 FUER EINEN BESTIMMTEN ZWECK. Details finden Sie in der GNU General Public License.

 Sie sollten ein Exemplar der GNU General Public License zusammen mit diesem
 Programm erhalten haben. Falls nicht, siehe:
 http://www.gnu.org/licenses/ (Englisch)
 http://www.gnu.de/documents/gpl.de.html (Deutsch)
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

String scanner = "";  // This buffer-string keeps the http-request from the webclient      // scanner = "***\n"; 

void setup() {
  
   for(int i=2; i<=9; i++)  // Define pin 2 - 9 as output
    pinMode(i, OUTPUT);
    
 // Open serial communications and wait for port to open:
  Serial.begin(38400); // !! Change it if you need !!
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

        // if you've gotten to the end of the line (received a newline character) and the line is blank, the http request has ended, so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          
          scanner += "";  //  scanner += "***";
          
          controller(scanner);
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Access-Control-Allow-Origin: *");
          client.println("Content-Type: application/xml");
          client.println("Connnection: close");
          client.println();
          client.println("<?xml version=\"1.0\"?>");  // here starts the definition for xml-output-string
          client.println("<root>");  // this is the root-node
          // add a meta refresh tag, so the browser pulls again every 5 seconds:
          // client.println("<meta http-equiv=\"refresh\" content=\"5\">");
          // output the value of each analog input pin
          
          // this loop writes the state of the digital-outputs to the xml-output-String
          for (int digitalChannel = 2; digitalChannel <= 9; digitalChannel++) {
            int sensorReading = digitalRead(digitalChannel);
            client.print("\t<data name=\"dout_");
            client.print(digitalChannel);
            client.print("\" id=\"dout_");
            client.print(digitalChannel);
            client.print("\">");
            client.print(sensorReading);
            client.println("</data>\n");
          }
          
          // this loop writes the state of the adc-values to the xml-output-String
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("\t<data name=\"adc_");
            client.print(analogChannel);
            client.print("\">");
            client.print(sensorReading);
            client.println("</data>\n");       
          }
          client.println("</root>");  // end of the root-node
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
     
    scanner = "";  // clear the buffer-String
  }
}

/* The controller. Main job is to handle the digitalWrite
   and analogRead commands from the webclient
*/
void controller(String string)
{
  if(string.indexOf("ardcmd:dw:") >= 0)  // test, if client calls a digitalWrite-command
  {
    String search = "ardcmd:dw:";                 // the search-expression
    int startsAt = string.indexOf(search);        // looking for the start-position of the search-expression
    int endsAt = string.indexOf(";", startsAt+1); // looking for the end-position of the command
    int pin, pinState;                            // holds the pin-number and the pin-state
    char part[10];
    String part1, part2;
    
    part1 = string.substring(startsAt+search.length(), startsAt+search.length()+1); // return the first value as string
    part2 = string.substring(startsAt+search.length()+2, endsAt);                   // return the second value as string
    
    /*
    Serial.print("\nardcmd:dw: vorhanden\n");
    
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
     
     digitalWrite(pin, pinState);  // set the selected pin-states
  }
  
  // special functions for adc-values are still in brainstorming
  if(string.indexOf("ardcmd:adc:") >= 0)
  {
    
  }
}
