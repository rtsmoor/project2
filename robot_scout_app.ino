#include <WiFi.h>


// Replace with your network credentials
const char* ssid     = "FRITZ!Box 7560 RS";
const char* password = "h2803ouderling33ggin";

/*
  const char* ssid     = "FRITZ!Box 7560 TX";
  const char* password = "83180663747509621361";
*/

/*
  const char* ssid     = "Tesla IoT";
  const char* password = "fsL6HgjN";
*/

// Set web server port number to 80
WiFiServer server(80);

#define motorLinksVooruit 27
#define motorRechtsVooruit 26
#define motorLinksAchteruit 25
#define motorRechtsAchteruit 33

#define ultrasoon_sensorLinks 32
#define ultrasoon_sensorRechts 35

#define infrarood_sensorLinks 32
#define infrarood_sensorRechts 35

String header;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs

  //Outputs and inputs
  pinMode(motorLinksVooruit, OUTPUT);
  pinMode(motorRechtsVooruit, OUTPUT);
  pinMode(motorLinksAchteruit, OUTPUT);
  pinMode(motorRechtsAchteruit, OUTPUT);

  pinMode(ultrasoon_sensorRechts, OUTPUT);
  pinMode(ultrasoon_sensorLinks, OUTPUT);

  pinMode(infrarood_sensorLinks, OUTPUT);
  pinMode(infrarood_sensorRechts, OUTPUT);

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




}


void loop() {
  // Check if client connected
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) { // If there is a client...
    boolean currentLineIsBlank = true;
    String buffer = ""; // A buffer for the GET request

    while (client.connected()) {

      if (client.available()) {
        char c = client.read();// Read the data of the client
        buffer += c; // Store the data in a buffer

        if (c == '\n' && currentLineIsBlank) {// if 2x new line ==> Request ended
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println(); // Blank line ==> end response
          break;
        }
        if (c == '\n') { // if New line
          currentLineIsBlank = true;
          buffer = "";  // Clear buffer
        }
        else if (c == '\r') { // If cariage return...
          //Read in the buffer if there was send "GET /?..."
          if (buffer.indexOf("GET /?vooruit=1") >= 0) { // If led1 = 1
            digitalWrite(motorLinksVooruit, HIGH); // led 1 > on
            digitalWrite(motorRechtsVooruit, HIGH); // led 1 > on
            digitalWrite(motorLinksAchteruit, LOW); // led 1 > on
            digitalWrite(motorRechtsAchteruit, LOW); // led 1 > on
            Serial.println("Vooruit");
          }
          if (buffer.indexOf("GET /?achteruit=1") >= 0) { // If led1 = 1
            digitalWrite(motorLinksVooruit, LOW); // led 1 > on
            digitalWrite(motorRechtsVooruit, LOW); // led 1 > on
            digitalWrite(motorLinksAchteruit, HIGH); // led 1 > on
            digitalWrite(motorRechtsAchteruit, HIGH); // led 1 > on
            Serial.println("Achteruit");
          }
          if (buffer.indexOf("GET /?links=1") >= 0) { // If led1 = 1
            digitalWrite(motorLinksVooruit, LOW); // led 1 > on
            digitalWrite(motorRechtsVooruit, HIGH); // led 1 > on
            digitalWrite(motorLinksAchteruit, LOW); // led 1 > on
            digitalWrite(motorRechtsAchteruit, LOW); // led 1 > on
            Serial.println("Rechts");
          }
          if (buffer.indexOf("GET /?rechts=1") >= 0) { // If led1 = 1
            digitalWrite(motorLinksVooruit, HIGH); // led 1 > on
            digitalWrite(motorRechtsVooruit, LOW); // led 1 > on
            digitalWrite(motorLinksAchteruit, LOW); // led 1 > on
            digitalWrite(motorRechtsAchteruit, LOW); // led 1 > on
            Serial.println("Rechts");
          }
          else {
            digitalWrite(motorLinksVooruit, LOW);
            digitalWrite(motorRechtsVooruit, LOW);
            digitalWrite(motorLinksAchteruit, LOW);
            digitalWrite(motorRechtsAchteruit, LOW);
          }
        }
        else {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
  }
}
