#include <WiFi.h>
#include <Arduino.h>

const int trigPin = 5;
const int echoPin = 18;

// define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceMm;
float distanceInch;

const char *ssid = "RESEAU_IDO";
const char *password = "12345678";

const uint16_t HEARTBEAT_TIMEOUT = 10000;
const uint16_t TRANSITION_TIME_DELAY = 3000;

const uint8_t DATAITEMS_NB = 1;
const String DATAITEM_IDS[] = {"d1"};

String incoming = "";
boolean alreadyConnected = false;
double currState[DATAITEMS_NB];

WiFiServer server(7878);
WiFiClient client;

void setup()
{
  Serial.begin(115200);
  delay(1000);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);  // Sets the echoPin as an Input

  WiFi.begin(ssid, password);
  Serial.print("Connexion à ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connecté");
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP());

  server.begin();
  Serial.println("Serveur TCP lancé sur le port 7878");
}

void sendSHDRStringData(String dataitemId, double value)
{
  client.println("|" + dataitemId + "|" + value + "\n");
  Serial.println("Sent: |" + dataitemId + "|" + value + "\n");
}

double measureDistance()
{
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance
  distanceCm = duration * SOUND_SPEED / 2;

  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;

  distanceMm = 10 * distanceCm;

  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  Serial.print("Distance (inch): ");
  Serial.println(distanceInch);

  return distanceMm;
}

void loop()
{
  if (!client || !client.connected())
  {
    client = server.available(); // ou server.accept();
  }

  if (client && client.connected())
  {
    Serial.println("New client");
    while (client.connected())
    {
      if (!alreadyConnected)
      {
        // send initial SHDR data on first connection
        if (WiFi.status() == WL_CONNECTED)
        {
          client.println("|avail|AVAILABLE\n");
          Serial.println("Sent: |avail|AVAILABLE\n");
        }
        else
        {
          client.println("|avail|UNAVAILABLE\n");
          Serial.println("Sent: |avail|UNAVAILABLE\n");
        }
        client.println("* shdrVersion: 2.0\n");
        Serial.println("Sent: * shdrVersion: 2.0\n");

        client.println("* adapterVersion: 2.0\n");
        Serial.println("Sent: * adapterVersion: 2.0\n");

        for (int i = 0; i < DATAITEMS_NB; i++)
        {
          currState[i] = (measureDistance());
          sendSHDRStringData(DATAITEM_IDS[i], currState[i]);
        }

        alreadyConnected = true;
      }
      // send data when state changes
      else
      {
        double newState[DATAITEMS_NB];

        for (int i = 0; i < DATAITEMS_NB; i++)
        {
          newState[i] = (measureDistance());

          if (newState[i] != currState[i])
          {
            delay(TRANSITION_TIME_DELAY);
            sendSHDRStringData(DATAITEM_IDS[i], newState[i]);
            currState[i] = newState[i];
          }
        }
      }

      // Check for PING and answer PONG if its the case
      if (client.available())
      {
        char c = client.read();
        incoming += c;

        if (incoming.indexOf("* PING") >= 0)
        {
          client.println("* PONG " + String(HEARTBEAT_TIMEOUT) + "\n");
          Serial.println("Sent: * PONG " + String(HEARTBEAT_TIMEOUT) + "\n");
          incoming = "";
        }
      }
      delay(1000);
    }
    client.stop();
    Serial.println("client disconnected");
    alreadyConnected = false;
  }
}