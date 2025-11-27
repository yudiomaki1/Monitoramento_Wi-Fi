#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>


const char* ssid = "Inteli.Iot"; 
const char* password = "%(Yk(sxGMtvFEs.3";

// CONFIGURAÇÕES DO HIVEMQ CLOUD 
const char* mqtt_server = "ce8024cdd8084f91b9f0e896819b210b.s1.eu.hivemq.cloud"; 
const int mqtt_port = 8883; // Porta segura (SSL)
const char* mqtt_user = "hivemq.webclient.1764252624556"; // Usuário que você criou na aba 'Access Management'
const char* mqtt_password = "4&rl3%a?iMRx5SO,yYJ6";

// Tópico onde publicaremos os dados
const char* topic_publish = "inteli/projeto/rssi";

WiFiClientSecure espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  50
char msg[MSG_BUFFER_SIZE];

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("conectado ao HiveMQ!");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  
  espClient.setInsecure(); 
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  // Publica a cada 1 segundo (1000ms)
  if (now - lastMsg > 1000) {
    lastMsg = now;
    
    // 1. Ler a potência do sinal (RSSI)
    long rssi = WiFi.RSSI();

    // 2. Imprimir na Serial (como pedido no enunciado)
    Serial.print("Sinal WiFi (dBm): ");
    Serial.println(rssi);

    // 3. Publicar no HiveMQ
    // Converter o número para string
    snprintf (msg, MSG_BUFFER_SIZE, "%ld", rssi);
    
    // Publica no tópico
    client.publish(topic_publish, msg);
  }
}
