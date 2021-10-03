#include <WiFi.h>
#include <PubSubClient.h>

#include <config.h>
// or
//const char* ssid = "";
//const char* password = "";

const char* mqtt_server = "192.168.1.100";
const int mqttPort = 1884; //port utilisé par le Broker 

long tps=0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  setup_wifi();
  setup_mqtt();
  client.publish("esp/test", "Hello from ESP32");
}

void loop() {
  reconnect();
  client.loop(); 
  //On utilise pas un delay pour ne pas bloquer la réception de messages 
  if (millis()-tps>10000){
     tps=millis();
     float temp = random(30);
     mqtt_publish("esp/test/from_esp",temp);
     Serial.print("qqchose : ");
     Serial.println(temp); 
   }
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connecté");
  Serial.print("MAC : ");
  Serial.println(WiFi.macAddress());
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP());
}

void setup_mqtt(){
  client.setServer(mqtt_server, mqttPort);
  client.setCallback(callback);//Déclaration de la fonction de souscription
  reconnect();
}


void reconnect(){
  while (!client.connected()) {
    Serial.println("Connection au serveur MQTT ...");
    if (client.connect("ESP32Client")) {
      Serial.println("MQTT connecté");
    }
    else {
      Serial.print("echec, code erreur= ");
      Serial.println(client.state());
      Serial.println("nouvel essai dans 2s");
    delay(2000);
    }
  }
  client.subscribe("esp/test/to_esp");//souscription au topic led pour commander une led
}


//Callback doit être présent pour souscrire a un topic et de prévoir une action 
void callback(char* topic, byte *payload, unsigned int length) {
   Serial.println("-------Nouveau message du broker mqtt-----");
   Serial.print("Canal:");
   Serial.println(topic);
   Serial.print("donnee:");
   Serial.write(payload, length);
   Serial.println();

   
   if ((char)payload[0] == '1') {
     Serial.println("LED ON");
      digitalWrite(2,HIGH); 
   } else {
     Serial.println("LED OFF");
     digitalWrite(2,LOW); 
   }
 }


//Fonction pour publier un float sur un topic 
void mqtt_publish(String topic, float t){
  char top[topic.length()+1];
  topic.toCharArray(top,topic.length()+1);
  char t_char[50];
  String t_str = String(t);
  t_str.toCharArray(t_char, t_str.length() + 1);
  client.publish(top,t_char);
}
