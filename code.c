#include <ESP8266WiFi.h>
#include <PubSubClient.h>

int BUTTON_PIN = D3; //button is connected to GPIO pin D3
// Update these with values suitable for your network.
const char* ssid = ""; //SSID WIFI Access Point
const char* password = ""; //Access Point Password
const char* mqtt_server = ""; //IP address MQTT server

//MQTT Configuration
WiFiClient espClient; //Creates a partially initialised client instance.
PubSubClient client(espClient); //Before it can be used, the server details must be configured
long lastMsg = 0;
char msg[50];

//int buttonState =EEPROM.read(2); 
int buttonState =0;
int lastButtonState = 0;

void setup_wifi() {
   delay(100);
  // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  
} //end callback

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
     //once connected to MQTT broker, subscribe command if any
      client.subscribe("ElectricityStatus");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
} //end reconnect()

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(BUTTON_PIN,INPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
 
  //send message every 2 second
  if (now - lastMsg > 2000) {
     lastMsg = now;
     buttonState=digitalRead(BUTTON_PIN);
     String msg="";
   if (buttonState != lastButtonState) {
      if (buttonState == HIGH) {
           msg="1";
           char message[58];
           msg.toCharArray(message,58);
           Serial.println(message);
           //publish sensor data to MQTT broker
          client.publish("ElectricityStatus", message);
     }
     else 
     {
       msg= "0";
       char message[58];
       msg.toCharArray(message,58);
       Serial.println(message);
       //publish sensor data to MQTT broker
      client.publish("ElectricityStatus", message);
     }
     lastButtonState = buttonState;
  }
  }
}
