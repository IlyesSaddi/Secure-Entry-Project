#include <WiFi.h>
#include <WebServer.h>

// Nom du Wi-Fi que l’ESP va créer
const char* ssid = "ESP32_AP";
const char* password = "12345678";  // Minimum 8 caractères !

WebServer server(80);  // Serveur sur le port HTTP

// Quand un client accède à la racine "/"
void handleRoot() {
  server.send(200, "text/html", "<h1>Bienvenue sur le réseau ESP32 !</h1>");
}

void setup() {
  Serial.begin(115200);
  pinMode(23, OUTPUT); // Set GPIO 2 as output
  digitalWrite(23, HIGH); // Turn LED ON
  Serial.println("Démarrage du point d’accès...");

  // Lancer le point d’accès
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("Point d’accès démarré. IP : ");
  Serial.println(IP);

  // Lier la fonction handleRoot à l’URL "/"
  server.on("/", handleRoot);

  // Démarrer le serveur web
  server.begin();
  Serial.println("Serveur web actif.");
}

void loop() {
  server.handleClient();  // Gère les requêtes entrantes
}
