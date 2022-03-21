/*
Kria Kitchen Lifeguard Wireless Thermal Sensor Alarm
for ESP32

Author: Enrique Albertos
*/

#include <WiFi.h>
#include <WiFiMulti.h>
#include <Adafruit_MLX90640.h>

#include "secrets.h"
#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif

// ESP32 GIOP18 pin connected to Buzzer's pin
#define BUZZER_PIN 18 

// temperature threshold to trigger high temperature alerts on appliance 
#define TEMPERATURE_THRESHOLD 41 

// absent person time threshold in seconds
#define ABSENT_TIME_THRESHOLD 120 

// time between connections to the Kria Lifeguard People Detection System in milliseconds
#define CONN_WAITING_TIME 5000

Adafruit_MLX90640 mlx;
// buffer for full frame of temperatures
float frame[32 * 24];


// Set web server port number to 80
WiFiServer server(80);
// Variable to store the HTTP request
String header;
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;



const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "192.168.2.95";
const uint16_t port = 8000;

float maxtemp = -99.0;
String peopleDetectionState = "";

WiFiMulti WiFiMulti;

void setup() {
  setupSerial()
  setupWifi();
  setupWebServer();
  setupMLX90640();
}

void setupSerial() {
    Serial.begin(115200);
}
void setupWebServer() {
    server.begin();
}

void tempAlarmOn() {
  digitalWrite(BUZZER_PIN, HIGH); // turn on
}

void tempAlarmOff() {
  digitalWrite(BUZZER_PIN, LOW); // turn on
}

void setupWifi() {
  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);
}

void setupMLX90640() {

  delay(100);

  Serial.println("MLX90640 Simple Test");
  if (! mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
    Serial.println("MLX90640 not found!");
    while (1) delay(10);
  }
  Serial.println("Found MLX90640");

  Serial.print("Serial number: ");
  Serial.print(mlx.serialNumber[0], HEX);
  Serial.print(mlx.serialNumber[1], HEX);
  Serial.println(mlx.serialNumber[2], HEX);

  //mlx.setMode(MLX90640_INTERLEAVED);
  mlx.setMode(MLX90640_CHESS);
  Serial.print("Current mode: ");
  if (mlx.getMode() == MLX90640_CHESS) {
    Serial.println("Chess");
  } else {
    Serial.println("Interleave");
  }

  mlx.setResolution(MLX90640_ADC_18BIT);
  Serial.print("Current resolution: ");
  mlx90640_resolution_t res = mlx.getResolution();
  switch (res) {
    case MLX90640_ADC_16BIT: Serial.println("16 bit"); break;
    case MLX90640_ADC_17BIT: Serial.println("17 bit"); break;
    case MLX90640_ADC_18BIT: Serial.println("18 bit"); break;
    case MLX90640_ADC_19BIT: Serial.println("19 bit"); break;
  }

  mlx.setRefreshRate(MLX90640_2_HZ);
  Serial.print("Current frame rate: ");
  mlx90640_refreshrate_t rate = mlx.getRefreshRate();
  switch (rate) {
    case MLX90640_0_5_HZ: Serial.println("0.5 Hz"); break;
    case MLX90640_1_HZ: Serial.println("1 Hz"); break;
    case MLX90640_2_HZ: Serial.println("2 Hz"); break;
    case MLX90640_4_HZ: Serial.println("4 Hz"); break;
    case MLX90640_8_HZ: Serial.println("8 Hz"); break;
    case MLX90640_16_HZ: Serial.println("16 Hz"); break;
    case MLX90640_32_HZ: Serial.println("32 Hz"); break;
    case MLX90640_64_HZ: Serial.println("64 Hz"); break;
  }
}


void loop() {
  loopTcpClient();
  loopMLX();
  loopTcpServer();
}

void loopTcpServer() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
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
                       
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the web page
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Kitchen Life Guard Web Server</h1>");
            
            // Display current state
            client.println("<p>Last State " + peopleDetectionState + "</p>");
            client.println("<p>Actual Temperature " +  String(maxtemp, 1) + "</p>");

            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void loopTcpClient() {
  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    Serial.println("wait 5 sec...");
    delay(CONN_WAITING_TIME);
    return;
  }

  // This will send the request to the server
  //client.println("hello from ESP8266");

  //read back one line from server
  Serial.println("receiving from remote server");
  peopleDetectionState = client.readStringUntil('\n');
  Serial.println(peopleDetectionState);

  Serial.println("closing connection");
  client.stop();

  Serial.println("wait 5 sec...");
  delay(CONN_WAITING_TIME);
}

void loopMLX() {
  delay(500);
  if (mlx.getFrame(frame) != 0) {
    Serial.println("Failed");
    return;
  }
  Serial.println();
  Serial.println();
  
  for (uint8_t h = 0; h < 24; h++) {
    for (uint8_t w = 0; w < 32; w++) {
      float t = frame[h * 32 + w];
      if (t > maxtemp) {
        maxtemp = t;
      }
    }

  }
  Serial.print("MAX TEMP ");
  Serial.print(maxtemp, 1);   
  Serial.println();
  if( maxtemp > TEMPERATURE_THRESHOLD) {
    tempAlarmOn();
  } else {
    tempAlarmOff();
  }
}
