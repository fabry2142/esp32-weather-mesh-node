# ESP32 Weather Mesh Node & Mini-Server

An open-source Edge IoT project engineered around the **ESP32 (NodeMCU-32S)** microcontroller, designed to function as a data-collection mini-server and a resilient mesh network node.

## 📊 System Architecture
1. **Edge Integration:** The ESP32 acts as a localized mini-server, executing HTTP requests to external Weather APIs.
2. **Data Parsing & Management:** The firmware processes incoming JSON payloads locally at the edge level.
3. **Telemetry Visualization:** Telemetry data is pushed into an analytical pipeline and visualized through customized **Grafana** dashboards for real-time environmental monitoring.
4. **Mesh Networking:** Node-to-node routing capabilities to ensure data persistence across local infrastructure.

## 🚀 Technologies Used
* **Hardware:** ESP32 (NodeMCU-32S) Microcontroller
* **Core Concepts:** Edge Computing, Mesh Networking, API Integration (JSON Parsing)
* **Monitoring:** Grafana Analytics

* /**
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
