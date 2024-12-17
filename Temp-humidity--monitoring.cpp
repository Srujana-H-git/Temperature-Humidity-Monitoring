// void setup() {
//     pinMode(PC13, OUTPUT); // PC13 is the onboard LED pin
// }

// void loop() {
//     digitalWrite(PC13, HIGH); // Turn the LED on
//     delay(1000);              // Wait for a second
//     digitalWrite(PC13, LOW);  // Turn the LED off
//     delay(1000);              // Wait for a second
// }

 #include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DHT.h>

// Wi-Fi credentials
const char *ssid = "***********";
const char *password = "******";

// Server and DHT sensor configuration
WebServer server(80);
DHT dht(26, DHT11);

// Function to handle the root route
void handleRoot() {
  String temperature = isnan(readDHTTemperature()) ? "N/A" : String(readDHTTemperature(), 2);
  String humidity = isnan(readDHTHumidity()) ? "N/A" : String(readDHTHumidity(), 2);

  String html = "<html>\
  <head>\
    <meta http-equiv='refresh' content='4'/>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css' integrity='sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr' crossorigin='anonymous'>\
    <title>ESP32 DHT Server</title>\
    <style>\
    html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}\
    h2 { font-size: 3.0rem; }\
    p { font-size: 3.0rem; }\
    .units { font-size: 1.2rem; }\
    .dht-labels{ font-size: 1.5rem; vertical-align:middle; padding-bottom: 15px;}\
    </style>\
  </head>\
  <body>\
      <h2>ESP32 DHT Server!</h2>\
      <p>\
        <i class='fas fa-thermometer-half' style='color:#ca3517;'></i>\
        <span class='dht-labels'>Temperature</span>\
        <span>" + temperature + "</span>\
        <sup class='units'>&deg;C</sup>\
      </p>\
      <p>\
        <i class='fas fa-tint' style='color:#00add6;'></i>\
        <span class='dht-labels'>Humidity</span>\
        <span>" + humidity + "</span>\
        <sup class='units'>&percnt;</sup>\
      </p>\
  </body>\
</html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  // Initialize DHT sensor
  dht.begin();

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start MDNS responder
  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started.");
  }

  // Setup web server route
  server.on("/", handleRoot);

  // Start the server
  server.begin();
  Serial.println("HTTP server started.");
}

void loop() {
  server.handleClient();
  vTaskDelay(pdMS_TO_TICKS(2)); // Allow the CPU to perform other tasks
}

// Function to read temperature from the DHT sensor
float readDHTTemperature() {
  float t = NAN;
  for (int i = 0; i < 3 && isnan(t); i++) {
    t = dht.readTemperature();
    if (isnan(t)) delay(100); // Retry after a short delay
  }
  if (isnan(t)) {
    Serial.println("Failed to read temperature from DHT sensor!");
    return NAN;
  }
  Serial.print("Temperature: ");
  Serial.println(t);
  return t;
}

// Function to read humidity from the DHT sensor
float readDHTHumidity() {
  float h = NAN;
  for (int i = 0; i < 3 && isnan(h); i++) {
    h = dht.readHumidity();
    if (isnan(h)) delay(100); // Retry after a short delay
  }
  if (isnan(h)) {
    Serial.println("Failed to read humidity from DHT sensor!");
    return NAN;
  }
  Serial.print("Humidity: ");
  Serial.println(h);
  return h;
}
