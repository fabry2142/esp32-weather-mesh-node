/**
 * @file main.cpp
 * @brief ESP32 Weather Mesh Node & Mini-Server
 *
 * @section DESCRIPTION
 * Firmware per microcontrollori ESP32 (NodeMCU-32S) configurati per operare come 
 * nodi Edge IoT e mini-server di raccolta dati ambientali tramite API.
 *
 * @section DEPENDENCIES
 * Questo progetto richiede le seguenti librerie esterne:
 * - PubSubClient (v2.8.0) di Nick O'Leary [Per la gestione del protocollo MQTT]
 * - ArduinoJson (v7.x) di Benoit Blanchon [Per il parsing ottimizzato dei payload API]
 *
 * @section ESONERO DI RESPONSABILITÀ / DISCLAIMER
 * LICENZA MIT - COPYRIGHT (C) 2026
 * 
 * 1. L'autore declina ogni responsabilità per malfunzionamenti, interruzioni di servizio,
 *    danni a cose, dispositivi o infrastrutture derivanti dall'utilizzo, dall'installazione
 *    o dalla modifica di questo codice sorgente.
 * 
 * 2. Il software viene fornito "COSÌ COM'È" (AS IS), senza garanzie di alcun tipo, 
 *    esplicite o implicite, incluse, a titolo esemplificativo, garanzie di commerciabilità 
 *    o idoneità per uno scopo specifico.
 * 
 * 3. Questo codice è rilasciato esclusivamente per scopi didattici, di ricerca, studio 
 *    e Proof-of-Concept (PoC). Non è progettato né autorizzato per l'uso in ambienti di 
 *    produzione, sistemi industriali critici o applicazioni in tempo reale senza le dovute 
 *    fasi di validazione e hardening.
 * 
 * IN NESSUN CASO L'AUTORE SARÀ RESPONSABILE PER QUALSIASI RIVENDICAZIONE, DANNO O ALTRA 
 * RESPONSABILITÀ IN SEDE CONTRATTUALE, EXTRA-CONTRATTUALE O DERIVANTE DA ILLECITO, 
 * CAUSATI DALL'USO O DALLA MANIPOLAZIONE DEL PRESENTE SOFTWARE.
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <esp_now.h>

// --- CONFIGURAZIONE RETE & API ---
const char* ssid = "IL_TUO_WIFI_SSID";
const char* password = "LA_TUA_WIFI_PASSWORD";
const char* mqtt_server = "IP_DEL_TUO_DOCKER_MOSQUITTO"; 
const int mqtt_port = 1883;

// Endpoint OpenWeatherMap (Esempio: Messina, IT)
const String api_key = "LA_TUA_API_KEY_OPENWEATHER";
const String city = "Messina";
const String country_code = "IT";
const String api_url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + country_code + "&appid=" + api_key + "&units=metric";

// --- MAC INDIRIZZO PER MESH ESP-NOW (Esempio Nodo Successivo) ---
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// --- STRUTTURA DATI MESH ---
typedef struct struct_message {
    float temperature;
    int humidity;
    char node_id[16];
} struct_message;

struct_message meshData;
WiFiClient espClient;
PubSubClient mqttClient(espClient);

unsigned long lastTime = 0;
unsigned long timerDelay = 10000; // Intervallo di campionamento (10 secondi per test)

// --- CALLBACK INVIO ESP-NOW (MESH) ---
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\n[*] ESP-NOW Mesh Packet Sent Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("[*] Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n[+] WiFi Connected. IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect_mqtt() {
    while (!mqttClient.connected()) {
        Serial.print("[*] Attempting MQTT connection...");
        // Tentativo di connessione con ID client sicuro
        if (mqttClient.connect("ESP32_Weather_Server")) {
            Serial.println("\n[+] MQTT Connected to Broker.");
        } else {
            Serial.print("[-] Failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void fetch_weather_api() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(api_url);
        int httpResponseCode = http.GET();
        
        if (httpResponseCode > 0) {
            Serial.print("[+] HTTP Response code: ");
            Serial.println(httpResponseCode);
            String payload = http.getString();
            
            // Allocazione memoria per il parsing JSON (ottimizzata per ESP32)
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, payload);
            
            if (!error) {
                float temp = doc["main"]["temp"];
                int hum = doc["main"]["humidity"];
                
                Serial.printf("[📊] Local Data parsed -> Temp: %.2f°C | Hum: %d%%\n", temp, hum);
                
                // 1. Preparazione payload MQTT per Grafana
                String mqtt_payload = "{\"temperature\":" + String(temp) + ",\"humidity\":" + String(hum) + "}";
                mqttClient.publish("telemetry/weather/messina", mqtt_payload.c_str());
                
                // 2. Preparazione dati per la rete MESH (ESP-NOW)
                meshData.temperature = temp;
                meshData.humidity = hum;
                strcpy(meshData.node_id, "ESP32_NODE_01");
                
                esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &meshData, sizeof(meshData));
                if (result == ESP_OK) {
                    Serial.println("[🛡️] Mesh broadcast triggered successfully via ESP-NOW.");
                } else {
                    Serial.println("[-] Error sending Mesh packet.");
                }
            } else {
                Serial.print("[-] JSON Deserialization failed: ");
                Serial.println(error.f_str());
            }
        } else {
            Serial.print("[-] Error code on HTTP GET: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    }
}

void setup() {
    Serial.begin(115200);
    setup_wifi();
    
    // Configurazione MQTT
    mqttClient.setServer(mqtt_server, mqtt_port);
    
    // Inizializzazione protocollo MESH ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("[-] Error initializing ESP-NOW Mesh protocols.");
        return;
    }
    esp_now_register_send_cb(OnDataSent);
    
    // Registrazione peer Mesh
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("[-] Failed to add Mesh peer.");
        return;
    }
}

void loop() {
    if (!mqttClient.connected()) {
        reconnect_mqtt();
    }
    mqttClient.loop();

    unsigned long currentMillis = millis();
    if (currentMillis - lastTime >= timerDelay) {
        fetch_weather_api();
        lastTime = currentMillis;
    }
}