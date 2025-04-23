#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// Wi-Fi credentials
const char* ssid = "";
const char* password = "";

// Create server on port 80
WebServer server(80);

// Servo setup
Servo myServo;
const int servoPin = 18; // Connect signal wire here
int currentAngle = 0;

// Web page
const char* webpage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Servo Control</title>
  <style>
    body { background-color: black; color: white; text-align: center; padding-top: 50px; }
    .button {
      padding: 20px 40px;
      font-size: 24px;
      background-color: #E2AC0D;
      color: white;
      border: none;
      border-radius: 8px;
      cursor: pointer;
      margin: 10px;
    }
    #statusBox {
      margin-top: 30px;
      font-size: 20px;
      color: #00FF00;
    }
  </style>
</head>
<body>
  <h1>Servo Control Panel</h1>
  <button class="button" onclick="location.href='/on'">TURN ON</button>
  <button class="button" onclick="location.href='/off'">TURN OFF</button>
  <button class="button" onclick="location.href='/turnled_on'">turn on led</button>
  <button class="button" onclick="location.href='/turnled_off'">turn off led</button>
  <br><br>
  <input type="range" min="0" max="180" value="90" id="angleSlider" oninput="updateAngle(this.value)">
  <p>Angle: <span id="angleValue">90</span>°</p>
  <br><br>
  <button class="button" onclick="location.href='/angle180'">angle180</button>

<script>
  function updateAngle(val) {
    document.getElementById("angleValue").innerText = val;
    fetch("/setAngle?value=" + val);
  }
</script>

  <div id="statusBox">
    <p>Servo Angle: <span id="angle">--</span>°</p>
    <p>LED Status: <span id="led">--</span></p>
  </div>

  <script>
    function updateStatus() {
      fetch('/status')
        .then(response => response.json())
        .then(data => {
          document.getElementById("angle").textContent = data.angle;
          document.getElementById("led").textContent = data.led == 1 ? "ON" : "OFF";
        });
    }
    setInterval(updateStatus, 1000); // Refresh every 1 second
    updateStatus(); // Initial call
  </script>
</body>
</html>
)rawliteral";


// Route handlers
void handleRoot() {
  server.send(200, "text/html", webpage);
}

void handleOn() {
  myServo.write(0);  // Move to 0 degrees
  currentAngle = 0;
  Serial.println("Turn ON: Servo moved to 0°");
  server.send(200, "text/html", webpage);
}

void handleOff() {
  myServo.write(90);   // Move to 90 degrees
  currentAngle = 90;
  Serial.println("Turn OFF: Servo moved to 90°");
  server.send(200, "text/html", /*"<h1>Turned OFF (0°)</h1><a href='/'>Back</a>"*/ webpage);
}

void handleStatus() {
  String json = "{";
  json += "\"angle\":" + String(currentAngle) + ",";
  json += "\"led\":" + String(digitalRead(23));
  json += "}";
  server.send(200, "application/json", json);
}


void ledon(){
  digitalWrite(23, HIGH); // Turn LED ON
  Serial.println("led Turn ON");
  server.send(200, "text/html", /*"<h1>Turned OFF (0°)</h1><a href='/'>Back</a>"*/ webpage);
}
void ledoff(){
  digitalWrite(23, LOW); // Turn LED ON
  Serial.println("led Turn OFF");
  server.send(200, "text/html", /*"<h1>Turned OFF (0°)</h1><a href='/'>Back</a>"*/ webpage);
}
// slider mt3 servomoteur
void handleSetAngle() {
  if (server.hasArg("value")) {
    int angle = server.arg("value").toInt();
    angle = constrain(angle, 0, 180);
    myServo.write(angle);
    currentAngle = angle;
    Serial.printf("Servo angle set to %d°\n", angle);
  }
  server.send(200, "text/plain", "OK");
}

void turn180() {
  myServo.write(180); // Move to 180°
  delay(1000);
  myServo.write(90);  // Revenir à 90°
  currentAngle = 90;  // Mettre à jour l'angle
}

void setup() {
  Serial.begin(115200);
  pinMode(23, OUTPUT); // Set GPIO 2 as output
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

    // Attach servo
  myServo.setPeriodHertz(50); // 50Hz for analog servos
  myServo.attach(servoPin, 500, 2400); // Min/max pulse in µs

  // Setup web server
  //led
  server.on("/turnled_on",ledon);
  server.on("/turnled_off",ledoff);
  server.on("/angle180",turn180);
  //servomoteur
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff); 
  server.on("/setAngle", handleSetAngle);
  server.on("/status", handleStatus);  // auto refresh statutt !!! of servo

  server.begin();

  Serial.println("Web server started.");

  
}

void loop() {
  server.handleClient();
  
}