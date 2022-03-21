/*
    This sketch sends a string to a TCP server, and prints a one-line response.
    You must run a TCP server in your local network.
    For example, on Linux you can use this command: nc -v -l 3000
*/

#include <WiFi.h>
#include <WiFiMulti.h>
#include <Adafruit_MLX90640.h>

Adafruit_MLX90640 mlx;
float frame[32 * 24]; // buffer for full frame of temperatures

#include "secrets.h"
#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif
#define BUZZER_PIN 18 // ESP32 GIOP18 pin connected to Buzzer's pin
#define TEMPERATURE_THRESHOLD 41 // temperature threshold to trigger high temperature alerts on appliance 

#define ABSENT_TIME_THRESHOLD 120 // absent person time threshold in seconds

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "192.168.2.95";
const uint16_t port = 8000;

float maxtemp = -99.0;


WiFiMulti WiFiMulti;

void setup() {
  Serial.begin(115200);

  setupWifi();
  // WiFi.disconnect();
  setupMLX90640();
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
    delay(5000);
    return;
  }

  // This will send the request to the server
  //client.println("hello from ESP8266");

  //read back one line from server
  Serial.println("receiving from remote server");
  while (client.available()) {

    char c = client.read();

    Serial.write(c);

  }
  String line = client.readStringUntil('\n');
  Serial.println(line);

  Serial.println("closing connection");
  client.stop();

  Serial.println("wait 5 sec...");
  delay(5000);
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
