/*
  mtconnect_adapter

 MTConnect adapter for a smart shelf instrumentation, which detects the distance from the sensor to the top of the materials.
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13 if not using OPTA 
 * Each optocoupler attached to a separate digital in pin (values between A0 and A7).

 created 16 May 2025
 modified 30 May 2025
 */
 #include <Ethernet.h>

const uint16_t HEARTBEAT_TIMEOUT = 10000;
const uint16_t TRANSITION_TIME_DELAY = 3000;

const uint8_t DATAITEMS_NB = 2;
const String DATAITEM_IDS[] = {"A1ToolPlus", "A2ToolPlus"};
const uint8_t DATAITEM_PINS[] = {A0, A1};

String incoming = "";
boolean alreadyConnected = false;
String currState[DATAITEMS_NB];


// Static IP configuration for the Opta device.
IPAddress ip(10, 130, 22, 84);

EthernetServer server(7878);
EthernetClient client;

void setup() {
  Serial.begin(9600);

  while(!Serial);

  for(int i = 0; i < DATAITEMS_NB; i++){
    pinMode(DATAITEM_PINS[i], INPUT);
  }


  //Try starting Ethernet connection via DHCP
  if (Ethernet.begin() == 0) {
    Serial.println("- Failed to configure Ethernet using DHCP!");

    // Try to configure Ethernet with the predefined static IP address.
    Ethernet.begin(ip);
  }

   // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  server.begin();
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());

}

void loop() {
  // listen for incoming mtconnect agent
  client = server.accept();

  if(client)
  {
    Serial.println("New client");
    boolean currentLineIsBlank = true;
    while(client.connected())
    {
      if (!alreadyConnected)
      {
        //send initial SHDR data on first connection
        if(Ethernet.linkStatus() == LinkON)
        {
          client.println("|avail|AVAILABLE\n");
          Serial.println("Sent: |avail|AVAILABLE\n");
        } else
        {
          client.println("|avail|UNAVAILABLE\n");
          Serial.println("Sent: |avail|UNAVAILABLE\n");
        }
        client.println("* shdrVersion: 2.0\n");
        Serial.println("Sent: * shdrVersion: 2.0\n");

        client.println("* adapterVersion: 2.0\n");
        Serial.println("Sent: * adapterVersion: 2.0\n");

        for(int i = 0; i < DATAITEMS_NB; i++){
          currState[i] = (digitalRead(DATAITEM_PINS[i])?"OFF":"ON");
          sendSHDRStringData(DATAITEM_IDS[i], currState[i]);
        }

        alreadyConnected = true;
      }
      //send data when state changes
      else
      {
        String newState[DATAITEMS_NB];

        for(int i = 0; i < DATAITEMS_NB; i++){
          newState[i] = (digitalRead(DATAITEM_PINS[i])?"OFF":"ON");

          if(newState[i] != currState[i])
          {
            delay(TRANSITION_TIME_DELAY);
            sendSHDRStringData(DATAITEM_IDS[i], newState[i]);
            currState[i] = newState[i];
          }

        }

      }

      if (client.available())
      {
        char c = client.read();
        incoming += c;

        if(incoming.indexOf("* PING") >= 0)
        {
          client.println("* PONG " + String(HEARTBEAT_TIMEOUT) + "\n");
          Serial.println("Sent: * PONG " + String(HEARTBEAT_TIMEOUT) + "\n");
          incoming = "";
        }
      }
    }
    client.stop();
    Serial.println("client disconnected");
    alreadyConnected = false;
  }
}

void sendSHDRStringData(String dataitemId, String value)
{
  client.println("|"+ dataitemId + "|" + value + "\n");
  Serial.println("Sent: |"+ dataitemId + "|" + value + "\n");
}