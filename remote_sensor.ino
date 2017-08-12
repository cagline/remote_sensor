#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);
const int ledPin = 13; 
const int sw = 2;
int  var = 0;

boolean isDoorOpen = false;
boolean isDoorHasBeenOpen = false;


void setup() {
  // start serial port for debugging purposes
  Serial.begin(9600);
  
  // Attach interrupt to pin 2
  attachInterrupt(0, setDoorHasBeenOpen, RISING);

  pinMode(ledPin, OUTPUT);
  pinMode(sw, INPUT);
  
  // attempt to connect to Wifi network:
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  
  var = digitalRead(sw);
  Serial.println(var);
  if(var == HIGH){
    digitalWrite(ledPin, LOW);  
    setDoorOpen(false);
  }  
  if(var == LOW){
    digitalWrite(ledPin, HIGH); 
    setDoorOpen(true);
  } 
  
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<meta http-equiv=\"refresh\" content=\"5\">");

          // attach Interrupt
          if (isDoorHasBeenOpen == false){
            client.print("Everything is ok");
          }
          else {
             client.print("Alert ! The door has been opened");
          } 

          if(getDoorOpen()){
            client.print("Now door is Open");
            client.println("<br />");
          }
          else{
            client.print("Now door is Closed");
            client.println("<br />");
          }
            
          client.println("<br />");
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
      Serial.println("client disonnected");
  }
}

void setDoorHasBeenOpen() {
  isDoorHasBeenOpen = true;
}

void setDoorOpen(bool ds) {
  isDoorOpen = ds;
}
bool getDoorOpen() {
  return isDoorOpen;
}
