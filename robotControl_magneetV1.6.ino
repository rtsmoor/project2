// Load Wi-Fi library
#include <WiFi.h>

//Networkdata
const char* ssid     = "Tesla IoT";
const char* password = "fsL6HgjN";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String outputForwardState = "off";
String outputBackwardState = "off";
String outputLeftState = "off";
String outputRightState = "off";

//Variable of the state of the robot: Automatic-driving or manually controlled
// modus = Automatic-driving/Manually controlled
String modus = "Automatic-driving";


//Motors and sensors pins
#define motorLinksVooruit 27
#define motorRechtsVooruit 26
#define motorLinksAchteruit 14
#define motorRechtsAchteruit 25

#define ultrasoonSensorLinksTrig 13
#define ultrasoonSensorLinksEcho 34
#define ultrasoonSensorRechtsTrig 32
#define ultrasoonSensorRechtsEcho 33

#define infraroodSensorLinks 5
#define infraroodSensorRechts 21

#define LED 15

volatile int reedSensor = 23;

float tijd;               // variable in decimalen
float afstand;            // variable in decimalen

//Counter die aantal slachtoffers bij houdt
int counter = 0;
String detectie = "NO";
String obstacle = "NO";


// afgrond detectie
const int trigPinA = 19;
const int echoPinA = 18;

bool afgrond = false;
bool afgrondBuffer = false;
int bochtNaarLinks = 1;

void robotForward();
void robotLeft();
void robotSharpLeft();
void robotRight();
void robotSharpRight();
void robotReverse();
void robotStop();




void setup() {
  //
  Serial.begin(115200);
  // afgrond detectie
  pinMode(trigPinA, OUTPUT);
  pinMode(echoPinA, INPUT);

  // Initialize the output variables as outputs
  pinMode(motorLinksVooruit, OUTPUT);
  pinMode(motorRechtsVooruit, OUTPUT);
  pinMode(motorLinksAchteruit, OUTPUT);
  pinMode(motorRechtsAchteruit, OUTPUT);

  pinMode(ultrasoonSensorLinksTrig, OUTPUT);
  digitalWrite(ultrasoonSensorLinksTrig, LOW);
  pinMode(ultrasoonSensorRechtsTrig, OUTPUT);
  digitalWrite(ultrasoonSensorRechtsTrig, LOW);
  pinMode(ultrasoonSensorLinksEcho, INPUT);
  pinMode(ultrasoonSensorRechtsEcho, INPUT);

  pinMode(infraroodSensorLinks, INPUT_PULLUP);
  pinMode(infraroodSensorRechts, INPUT);

  pinMode(LED, OUTPUT);
  pinMode(reedSensor, INPUT_PULLUP);

  //Write all motors low
  digitalWrite(motorLinksVooruit, LOW);
  digitalWrite(motorRechtsVooruit, LOW);
  digitalWrite(motorLinksAchteruit, LOW);
  digitalWrite(motorRechtsAchteruit, LOW);

  digitalWrite(LED, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();

  robotForward();

}

void loop() {

  attachInterrupt(reedSensor, reedISR, CHANGE);

  static unsigned long startTime = millis();
  static unsigned long startTime2 = millis();
  static unsigned long startTime3 = millis();

  //---------------------------------- nieuwe code hier----------------------------
  //Serial.println(WiFi.localIP());
  if (modus == "Automatic-driving") {

    //Serial.println("Automatic-driving");
    //Serial.println("lijnDetectie");
    lijnDetectie();
    //Serial.println("afgrondDetectie");
    afgrondDetectie();
    //Serial.println("objectDetectie");
    objectDetectie();
    //Serial.println("Klaar");
    Serial.println(digitalRead(reedSensor));

    if (millis () - startTime > 500) {
      startTime = millis();

    }
    /*
          digitalWrite(trigPin, LOW);     // Zeker weten dan de trigPin geen signaal uitzend
          delayMicroseconds(2);           // wacht 2 miliseconden
          digitalWrite(trigPin, HIGH);    // De trigPin zendt een signaal (geluidsgolf) uit.
          delayMicroseconds(10);          // wacht 10 milisecomden
          digitalWrite(trigPin, LOW);     // De trigPin zendt geen sigaanl meer uit.

          tijd = pulseIn(echoPin, HIGH);  // De tijd is zolang van het moment van het uitzenden
          // van een signaal door de trigPin tot de echoPin een
          // signaal krijgt en dus HIGH wordt.
          afstand = (tijd * .0343) / 2;   // Met deze tijd berekent hij om m.b.v. de bovengenoemde
          // formule de afstand.

          if (millis () - startTime > 500) {
          startTime = millis();
          Serial.println(afstand);
          }
    */
  }
  // -----------------------------nieuwe code tot hier----------------------
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();


            //----------------------------------//Methods to get the data from the webserver //----------------------------------//

            //Header to select the modus
            if (header.indexOf("GET /condition/on") >= 0) {
              Serial.println("Automatic-driving");
              modus = "Automatic-driving";
              Serial.println("Automatic-driving");

              break;
            }
            if (header.indexOf("GET /condition/off") >= 0) {
              Serial.println("Manually controlled");
              modus = "Manually controlled";
              robotStop();
              Serial.println("Manually controlled");
            }

            if (modus == "Manually controlled") {


              //Header to drive the car forward
              if (outputForwardState == "off") {
                if (header.indexOf("GET /vooruit/on") >= 0) {
                  Serial.println("Vooruit start");
                  outputForwardState = "on";
                  digitalWrite(motorLinksAchteruit, LOW);
                  digitalWrite(motorRechtsVooruit, HIGH);
                  digitalWrite(motorLinksVooruit, HIGH);
                  digitalWrite(motorRechtsAchteruit, LOW);
                }
              }
              if (outputForwardState == "on") {
                if (header.indexOf("GET /vooruit/off") >= 0) {
                  Serial.println("Vooruit stop");
                  outputForwardState = "off";
                  digitalWrite(motorLinksAchteruit, LOW);
                  digitalWrite(motorRechtsVooruit, LOW);
                  digitalWrite(motorLinksVooruit, LOW);
                  digitalWrite(motorRechtsAchteruit, LOW);
                }
              }

              //Header to drive the car backward
              if (outputBackwardState == "off") {
                if (header.indexOf("GET /achteruit/on") >= 0)  {
                  Serial.println("Achteruit start");
                  outputBackwardState = "on";
                  digitalWrite(motorLinksAchteruit, HIGH);
                  digitalWrite(motorRechtsVooruit, LOW);
                  digitalWrite(motorLinksVooruit, LOW);
                  digitalWrite(motorRechtsAchteruit, HIGH);
                }
              }
              if (outputBackwardState == "on" ) {
                if (header.indexOf("GET /achteruit/off") >= 0) {
                  Serial.println("Achteruit stop");
                  outputBackwardState = "off";
                  digitalWrite(motorLinksAchteruit, LOW);
                  digitalWrite(motorRechtsVooruit, LOW);
                  digitalWrite(motorLinksVooruit, LOW);
                  digitalWrite(motorRechtsAchteruit, LOW);
                }
              }

              //Header to drive the car to the left
              if (outputLeftState == "off") {
                if (header.indexOf("GET /links/on") >= 0) {
                  Serial.println("Links start");
                  outputLeftState = "on";
                  digitalWrite(motorLinksAchteruit, HIGH);
                  digitalWrite(motorRechtsVooruit, HIGH);
                  digitalWrite(motorLinksVooruit, LOW);
                  digitalWrite(motorRechtsAchteruit, LOW);

                }
              }
              if (outputLeftState == "on") {
                if (header.indexOf("GET /links/off") >= 0) {
                  Serial.println("Links stop");
                  outputLeftState = "off";
                  digitalWrite(motorLinksAchteruit, LOW);
                  digitalWrite(motorRechtsVooruit, LOW);
                  digitalWrite(motorLinksVooruit, LOW);
                  digitalWrite(motorRechtsAchteruit, LOW);
                }
              }

              //Header to drive the car to the right
              if (outputRightState == "off") {
                if (header.indexOf("GET /rechts/on") >= 0) {
                  Serial.println("Rechts stop");
                  outputRightState = "on";
                  digitalWrite(motorLinksAchteruit, LOW);
                  digitalWrite(motorRechtsVooruit, LOW);
                  digitalWrite(motorLinksVooruit, HIGH);
                  digitalWrite(motorRechtsAchteruit, HIGH);
                }
              }
              if (outputRightState == "on") {
                if (header.indexOf("GET /rechts/off") >= 0) {
                  Serial.println("Rechts start");
                  outputRightState = "off";
                  digitalWrite(motorLinksAchteruit, LOW);
                  digitalWrite(motorRechtsVooruit, LOW);
                  digitalWrite(motorLinksVooruit, LOW);
                  digitalWrite(motorRechtsAchteruit, LOW);
                }
              }
            }


            //----------------------------------// Display the HTML web page //----------------------------------//



            client.println("<!DOCTYPE html><html>");
            client.println("<title>Robot control</title>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica;  text-align: center;}");
            client.println(".button { background-color: white; border: 2px solid #555555; color: black; padding: 16px 50px; font-family: \"Courier New\"; font-size: 1.4em;");
            client.println("-webkit-transition-duration: 0.4s; transition-duration: 0.4s;");
            client.println("text-decoration: none; font-size: 35px; margin: 2px; cursor: pointer;}");
            client.println(".button:hover {  background-color: #555555;  color: white;}");
            //client.println(".button1 {    position: absolute;  top: 200px;   text-align: center;  }");
            //client.println(".button2 {    position: absolute;  position: absolute; top: 40%; left: 50%;  margin-right: -50%; transform: translate(-50%, -50%) }");
            //Mclient.println(".button3 {    position: absolute;  left: 100px;  top: 350px;   text-align: center;  }");
            //client.println(".button4 {    position: absolute;    top: 450px;   text-align: center;  }");
            client.println("p { font-family: \"Courier New\"; font-size: 0.9em; user-select: none;}");
            client.println("a { font-family: \"Courier New\"; font-size: 0.9em; user-select: none;}");
            client.println("h1 { font-family: \"Courier New\"; font-size: 2.8em; user-select: none;");

            client.println("</style></head>");


            // Web Page Heading
            client.println("<body><h1>ROBOT CAR</h1>");

            client.println("<input type=\"text\" id=\"inputArrow\" autofocus autocomplete=\"off\">");

            // Current stats for the motors
            client.println("<p>Modus -  " + modus + "</p>");
            client.println("<p>Forward - State " + outputForwardState + "</p>");
            client.println("<p>Left - State " + outputLeftState + "</p>");
            client.println("<p>Right - State " + outputRightState + "</p>");
            client.println("<p>Backwards - State " + outputBackwardState + "</p>");

            //Buttons
            client.println("<a id=\"demoVooruit\"><button class=\"button button1\">Vooruit</button></a>");

            client.println("<div class=\"btn-group-vertical\">");
            client.println("<a id=\"demoLinks\"><button class=\"button button2 btn-group-vertical\">Links</button></a>");
            client.println("<a id=\"demoRechts\"><button class=\"button button3 btn-group-vertical\">Rechts</button></a>");
            client.println("</div>");

            client.println("<a id=\"demoAchteruit\"><button class=\"button button4\">Achteruit</button></a>");

            //Button to change the modus
            if (modus == "Automatic-driving") {
              client.println("<p><a href=\"/condition/off\"><button class=\"button\">Manually controlled</button></a></p>");
            }
            else {
              client.println("<p><a href=\"/condition/on\"><button class=\"button button5\">Automatic-driving</button></a></p>");
            }

            //Code for show detected victims
            client.println("<p>Present victum -  " + detectie + "</p>");
            client.println("<p>Detected victums - " + String(counter) + "</p>");


            //----------------------------------// Javascript code //----------------------------------//
            //client.println("");

            client.println("<script>");

            client.println("document.getElementById(\"inputArrow\").addEventListener(\"keydown\", logKeyDown);");
            client.println("document.getElementById(\"inputArrow\").addEventListener(\"keyup\", logKeyUp);");

            //Forward button
            client.println("document.getElementById(\"demoVooruit\").addEventListener(\"mousedown\", mouseDownVooruit);document.getElementById(\"demoVooruit\").addEventListener(\"mouseup\", mouseUpVooruit);");
            client.println("function mouseDownVooruit() {  location.href='/vooruit/on\'; }");
            client.println("function mouseUpVooruit() {  location.href='/vooruit/off\';  }");
            //Backward button
            client.println("document.getElementById(\"demoAchteruit\").addEventListener(\"mousedown\", mouseDownAchteruit);document.getElementById(\"demoAchteruit\").addEventListener(\"mouseup\", mouseUpAchteruit);");
            client.println("function mouseDownAchteruit() {  location.href='/achteruit/on\'; }");
            client.println("function mouseUpAchteruit() {  location.href='/achteruit/off\';}");
            //Left button
            client.println("document.getElementById(\"demoLinks\").addEventListener(\"mousedown\", mouseDownLinks); document.getElementById(\"demoLinks\").addEventListener(\"mouseup\", mouseUpLinks); ");
            client.println("function mouseDownLinks() {  location.href='/links/on\'; }");
            client.println("function mouseUpLinks() {  location.href='/links/off\';}");

            //Right button
            client.println("document.getElementById(\"demoRechts\").addEventListener(\"mousedown\", mouseDownRechts);document.getElementById(\"demoRechts\").addEventListener(\"mouseup\", mouseUpRechts);");
            client.println("function mouseDownRechts() {  location.href='/rechts/on\'; }");
            client.println("function mouseUpRechts() {  location.href='/rechts/off\';}");

            client.println("function logKeyDown(e) {  var key = e.which || e.keyCode || 0;");
            client.println("if (key == 37) {    mouseDownLinks() }");
            client.println("if (key == 38) {    mouseDownVooruit() }");
            client.println("if (key == 39) {    mouseDownRechts() }");
            client.println("if (key == 40) {    mouseDownAchteruit() }}");

            client.println("function logKeyUp(e) {  var key = e.which || e.keyCode || 0;");
            client.println("if (key == 37) {    mouseUpLinks() }");
            client.println("if (key == 38) {    mouseUpVooruit() }");
            client.println("if (key == 39) {    mouseUpRechts() }");
            client.println("if (key == 40) {    mouseUpAchteruit() }}");

            client.println("</script>");

            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop

            break;
          }
          else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        }
        else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    //Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
void lijnDetectie() {
  if (digitalRead(infraroodSensorLinks) == HIGH) {
    if (digitalRead(infraroodSensorRechts) == HIGH) {
      robotReverse();
      delay(500);
      robotSharpRight();
      delay(300);
      robotForward;
    }
    robotSharpRight();
    delay(300);
    robotForward();
  }
  else if (digitalRead(infraroodSensorRechts) == HIGH) {
    if (digitalRead(infraroodSensorLinks) == HIGH) {
      robotReverse();
      delay(500);
      robotSharpLeft();
      delay(300);
      robotForward;
    }
    robotSharpLeft();
    delay(300);
    robotForward();
  }
  else {
  }
}

void afgrondDetectie() {
  static unsigned long startTime3 = millis();
  static unsigned long startTime2 = millis();
  long duration;
  int distanceToGround;

  // Clears the trigPin
  digitalWrite(trigPinA, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds

  digitalWrite(trigPinA, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinA, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPinA, HIGH);

  // Calculating the distance
  distanceToGround = ((duration * 0.034) / 2) - 8;

  if (distanceToGround > 30) {
    distanceToGround = 30;
  }
  // Prints the distance on the Serial Monitor
  // Serial.print("Distance: ");
  // Serial.print(distanceToGround);
  // Serial.println(" cm");
  if (distanceToGround > 5) {
    afgrond = true;
  }
  if (distanceToGround <= 5) {
    afgrond = false;
  }

  if (afgrond == false && afgrondBuffer == false) {
    if (millis () - startTime3 > 2000) {

      startTime3 = millis();
      robotForward();
    }
  }

  if (afgrond == true) {

    robotReverse();
    delay(200);
    afgrondBuffer = true;
  }

  if (afgrondBuffer == true && afgrond == false) {
    robotReverse();


    robotStop();
    robotLeft();
    delay(500);
    afgrondBuffer = false;

  }

}

void objectDetectie() {
  static unsigned long startTime3 = millis();
  static unsigned long startTime2 = millis();
  long durationL, durationR;
  int distanceLeft, distanceRight;

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(ultrasoonSensorLinksTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasoonSensorLinksTrig, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  durationL = pulseIn(ultrasoonSensorLinksEcho, HIGH);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(ultrasoonSensorRechtsTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasoonSensorRechtsTrig, LOW);
  durationR = pulseIn(ultrasoonSensorRechtsEcho, HIGH);

  // Calculating the distance
  distanceLeft = ((durationL * 0.034) / 2) - 5 ;
  distanceRight = ((durationR * 0.034) / 2) - 5 ;

  if (distanceLeft > 30) {
    distanceLeft = 30;
  }
  if (distanceRight > 30) {
    distanceRight = 30;
  }
  // Prints the distance on the Serial Monitor
  /*Serial.print("Distance links: ");
    Serial.print(distanceLeft);
    Serial.println(" cm");
    // Prints the distance on the Serial Monitor
    Serial.print("Distance rechts: ");
    Serial.print(distanceRight);
    Serial.println(" cm");*/

  if (distanceRight > distanceLeft && distanceRight < 15) {
    robotReverse();
    delay(500);
    robotLeft();
    delay(500);
    robotForward();
  }
  if (distanceRight < distanceLeft && distanceLeft < 15) {
    robotReverse();
    delay(500);
    robotRight();
    delay(500);
    robotForward;
  }
  if (distanceRight == distanceLeft && distanceRight < 15) {
    robotReverse();
    delay(500);
    robotSharpLeft();
    delay(500);
    robotForward();
  }
}

void robotForward() {
  digitalWrite(LED, LOW);
  digitalWrite(motorLinksAchteruit, LOW);
  digitalWrite(motorRechtsAchteruit, LOW);
  digitalWrite(motorLinksVooruit, HIGH);
  digitalWrite(motorRechtsVooruit, HIGH);
}

void robotLeft() {
  digitalWrite(motorLinksAchteruit, LOW);
  digitalWrite(motorRechtsAchteruit, LOW);
  digitalWrite(motorLinksVooruit, LOW);
  digitalWrite(motorRechtsVooruit, HIGH);

}
void robotSharpLeft() {
  //Serial.println("sharpLeft");
  digitalWrite(motorRechtsAchteruit, LOW);
  digitalWrite(motorLinksVooruit, LOW);
  digitalWrite(motorRechtsVooruit, HIGH);
  digitalWrite(motorLinksAchteruit, HIGH);

}

void robotRight() {
  digitalWrite(motorLinksAchteruit, LOW);
  digitalWrite(motorRechtsAchteruit, LOW);
  digitalWrite(motorLinksVooruit, HIGH);
  digitalWrite(motorRechtsVooruit, LOW);
}
void robotSharpRight() {
  digitalWrite(motorLinksAchteruit, LOW);
  digitalWrite(motorRechtsAchteruit, HIGH);
  digitalWrite(motorLinksVooruit, HIGH);
  digitalWrite(motorRechtsVooruit, LOW);
}

void robotReverse() {
  digitalWrite(motorLinksVooruit, LOW);
  digitalWrite(motorRechtsVooruit, LOW);
  digitalWrite(motorLinksAchteruit, HIGH);
  digitalWrite(motorRechtsAchteruit, HIGH);
}

void robotStop() {
  digitalWrite(motorLinksAchteruit, LOW);
  digitalWrite(motorRechtsAchteruit, LOW);
  digitalWrite(motorLinksVooruit, LOW);
  digitalWrite(motorRechtsVooruit, LOW);
}

void reedISR() {
  
  digitalWrite(LED, HIGH);
  robotStop();
  
}
