/*
  Networking
  3D PARTY
*/

#include "CustomNetwork.hpp"
#include "enum.hpp"
#include <Arduino.h>
#include <cstdlib>

using namespace CustomNetwork;

// ESP-NOW Network Peer class
// This class represents a peer in the ESP-NOW network

ESP_NOW_Network_Peer::ESP_NOW_Network_Peer(const uint8_t *mac_addr,
    const uint8_t *lmk)
    : ESP_NOW_Peer(mac_addr, ESPNOW_WIFI_CHANNEL, ESPNOW_WIFI_IFACE, lmk) {
  this->last_data.resize(5, 0); // Initialize the vector with 5 elements set to 0
}

bool ESP_NOW_Network_Peer::begin() {
  // In this example the ESP-NOW protocol will already be initialized as we
  // require it to receive broadcast messages.
  if (!add()) {
    log_e("Failed to initialize ESP-NOW or register the peer");
    return false;
  }
  return true;
}

bool ESP_NOW_Network_Peer::sendPacket(esp_now_data_t packet) {
  Serial.println("Here hitler3");
  if (packet.service_id == ServiceId::NONE) {
    log_e("Service ID is NONE");
    return false;
  }

  packet.count = sent_packet_count;
  sent_packet_count++;
  Serial.println("sending packet");
  delay(100); // Simulate some delay for sending the packet
  // Call the parent class method to send the data
  return send((const uint8_t *)&packet, sizeof(packet));
}

void ESP_NOW_Network_Peer::onReceive(const uint8_t *data, size_t len,
                                     bool broadcast) {
  esp_now_data_t *packet = (esp_now_data_t *)data;

  if (!broadcast) {
    this->recv_packet_count++;
    Serial.printf("Received a message from peer " MACSTR "\n", MAC2STR(addr()));
    Serial.printf("  Count: %lu\n", packet->count);
    Serial.printf("  Payload: ");
    for (size_t i = 0; i < packet->payload_len; i++) {
      Serial.printf("%02X ", packet->payload[i]);
    }
    Serial.println();
    Serial.printf("  Service ID: %d\n", packet->service_id);
    Serial.printf("  Received packet count: %lu\n", recv_packet_count);
  }
}

void ESP_NOW_Network_Peer::onSent(bool success) {
  bool broadcast =
      memcmp(addr(), ESP_NOW.BROADCAST_ADDR, ESP_NOW_ETH_ALEN) == 0;

  if (broadcast) {
    log_i("Broadcast message reported as sent %s",
          success ? "successfully" : "unsuccessfully");
  } else {
    log_i("Unicast message reported as sent %s to peer " MACSTR,
          success ? "successfully" : "unsuccessfully", MAC2STR(addr()));
  }
}

// CustomNetworkManager class
// This class manages the ESP-NOW network and handles the peers

CustomNetworkManager::CustomNetworkManager()
    : broadcast_peer(ESP_NOW.BROADCAST_ADDR, NULL) {
}

void CustomNetworkManager::setup() {
  uint8_t self_mac[6];

  Serial.begin(115200);

  // Initialize the Wi-Fi module
  WiFi.mode(WIFI_STA);
  WiFi.setChannel(ESPNOW_WIFI_CHANNEL);
  while (!WiFi.STA.started()) {
    delay(100);
  }

  Serial.println("ESP-NOW Network Example");
  Serial.println("Wi-Fi parameters:");
  Serial.println("  Mode: STA");
  Serial.println("  MAC Address: " + WiFi.macAddress());
  Serial.printf("  Channel: %d\n", ESPNOW_WIFI_CHANNEL);

  WiFi.macAddress(self_mac);

  // Initialize the ESP-NOW protocol
  if (!ESP_NOW.begin((const uint8_t *)ESPNOW_EXAMPLE_PMK)) {
    Serial.println("Failed to initialize ESP-NOW");
    failReboot();
  }

  if (!broadcast_peer.begin()) {
    Serial.println("Failed to initialize broadcast peer");
    failReboot();
  }

  // Register the callback to be called when a new peer is found
  ESP_NOW.onNewPeer(CustomNetworkManager::registerNewPeer, this);

  Serial.println("Setup complete. Broadcasting...");
}

void CustomNetworkManager::update() {
  applyOutputMessages();
}

void CustomNetworkManager::pushOutputMessage(uint8_t *payload, size_t payload_len, ServiceId service_id, ConnectionType connection_type) {
  message_t msg;

  if (payload == NULL || payload_len == 0) {
    Serial.println("Payload is NULL or length is 0");
    return;
  }
  if (payload_len > ESP_NOW_MAX_DATA_LEN) {
    Serial.printf("Payload length exceeds maximum (%d > %d)\n", payload_len, ESP_NOW_MAX_DATA_LEN);
    return;
  }
  if (service_id == ServiceId::NONE) {
    Serial.println("Service ID is NONE");
    return;
  }

  Serial.println("Here 0");

  Serial.println("Here 1");
  
  msg.packet.payload_len = payload_len;
  Serial.println("Here 1.5");
  memset(msg.packet.payload, 0, ESP_NOW_MAX_DATA_LEN);
  memcpy(msg.packet.payload, payload, payload_len);
  msg.packet.service_id = service_id;
  Serial.println("Here 2");
  
  msg.connection_type = connection_type;
  _output_messages.push_back(msg);
  Serial.println("Here 3");

}

uint8_t *CustomNetworkManager::popInputMessage() {
  if (_input_messages.empty()) {
    return nullptr;
  }

  message_t msg = _input_messages.back();
  _input_messages.pop_back();

  uint8_t *payload = new uint8_t[msg.packet.payload_len];
  memcpy(payload, msg.packet.payload, msg.packet.payload_len);
  return payload;
}

void CustomNetworkManager::applyOutputMessages() {
  if (_output_messages.empty()) {
    return;
  }

  for (message_t &msg : _output_messages) {
  Serial.println("Here hitler");

    if (!sendMessage(msg)) {
      Serial.println("Failed to send message");
    }
  }
  _output_messages.clear();
}

bool CustomNetworkManager::sendMessage(message_t message) {
  bool isError = false;

  if (this->peer == nullptr) {
    Serial.println("No peer available to send the message");
    return false;
  }
  Serial.printf("Sending packet: count=%lu, payload_len=%d, service_id=%d\n",
          message.packet.count,
          message.packet.payload_len,
          message.packet.service_id);
  Serial.println("Here hitler2");

  if (!peer->sendPacket(message.packet)) {
    Serial.printf("Failed to send message to peer " MACSTR "\n",
                  MAC2STR(peer->addr()));
    isError = true;
  } else {
    Serial.printf("Sent message to peer " MACSTR "\n", MAC2STR(peer->addr()));
  }

  return !isError;
}

/* Helper functions */

// Function to reboot the device
void CustomNetworkManager::failReboot() {
  Serial.println("Rebooting in 5 seconds...");
  delay(5000);
  ESP.restart();
}

/* Callbacks */

// Callback called when a new peer is found
void CustomNetworkManager::registerNewPeer(const esp_now_recv_info_t *info,
      const uint8_t *data, int len, void *arg) {
  CustomNetworkManager *network = static_cast<CustomNetworkManager *>(arg);
  esp_now_data_t *packet = (esp_now_data_t *)data;
  ESP_NOW_Network_Peer *new_peer = nullptr;

  Serial.printf("New peer found: " MACSTR "\n", MAC2STR(info->src_addr));
  new_peer = new ESP_NOW_Network_Peer(info->src_addr);

  if (new_peer == nullptr || !new_peer->begin()) {
    Serial.println("Failed to create or register the new peer");
    delete new_peer;
    return;
  }
  network->peer = new_peer;
}
