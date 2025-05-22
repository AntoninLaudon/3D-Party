/*
  Networking
  3D PARTY
*/

#include <Arduino.h>
#include <vector>
#include "network.hpp"

std::vector<uint32_t>
  last_data;                  // Vector that will store the last 5 data received
uint32_t recv_msg_count = 0;  // Counter for the messages received. Only starts
// counting after all peers have been found

uint8_t current_peer_count = 0;  // Number of peers that have been found
uint32_t sent_msg_count = 0;     // Counter for the messages sent. Only starts
                                 // counting after all peers have been found

std::vector<ESP_NOW_Network_Peer *>
  peers;  // Create a vector to store the peer pointers

bool ESP_NOW_Network_Peer::begin() {
  // In this example the ESP-NOW protocol will already be initialized as we require it to receive broadcast messages.
  if (!add()) {
    log_e("Failed to initialize ESP-NOW or register the peer");
    return false;
  }
  return true;
}

bool ESP_NOW_Network_Peer::send_message(const uint8_t *data, size_t len) {
  if (data == NULL || len == 0) {
    log_e("Data to be sent is NULL or has a length of 0");
    return false;
  }

  // Call the parent class method to send the data
  return send(data, len);
}

void ESP_NOW_Network_Peer::onReceive(const uint8_t *data, size_t len, bool broadcast) {
  esp_now_data_t *msg = (esp_now_data_t *)data;

  if (peer_ready == false && msg->ready == true) {
    Serial.printf("Peer " MACSTR " reported ready\n", MAC2STR(addr()));
    peer_ready = true;
  }

  if (!broadcast) {
    recv_msg_count++;
    Serial.printf("Received a message from peer " MACSTR "\n", MAC2STR(addr()));
    Serial.printf("  Count: %lu\n", msg->count);
    Serial.printf("  Random data: %lu\n", msg->data);
    last_data.push_back(msg->data);
    last_data.erase(last_data.begin());
  }
}

void ESP_NOW_Network_Peer::onSent(bool success) {
  bool broadcast = memcmp(addr(), ESP_NOW.BROADCAST_ADDR, ESP_NOW_ETH_ALEN) == 0;

  if (broadcast) {
    log_i("Broadcast message reported as sent %s", success ? "successfully" : "unsuccessfully");
  } else {
    log_i("Unicast message reported as sent %s to peer " MACSTR, success ? "successfully" : "unsuccessfully", MAC2STR(addr()));
  }
}


MyNetwork::MyNetwork()
  : broadcast_peer(ESP_NOW.BROADCAST_ADDR) {
  // Constructor
  version = 0;
  current_peer_count = 0;
  sent_msg_count = 0;
  new_msg.count = 0;
  new_msg.data = 0;
  new_msg.ready = false;
  memset(new_msg.str, '\0', sizeof(new_msg.str));
  last_data.resize(5, 0);  // Initialize the vector with 5 elements set to 0
}

MyNetwork::~MyNetwork() {
  // Destructor
}

void MyNetwork::setup() {
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
    fail_reboot();
  }

  if (!broadcast_peer.begin()) {
    Serial.println("Failed to initialize broadcast peer");
    fail_reboot();
  }

  // Register the callback to be called when a new peer is found
  ESP_NOW.onNewPeer(MyNetwork::register_new_peer, this);

  Serial.println("Setup complete. Broadcasting...");
  memset(&new_msg, 0, sizeof(new_msg));
  strncpy(new_msg.str, "Hello!", sizeof(new_msg.str));
}

void MyNetwork::update() {
  // if (!master_decided) {
  //   // Broadcast the priority to find the master
  //   if (!broadcast_peer.send_message((const uint8_t *)&new_msg, sizeof(new_msg))) {
  //     Serial.println("Failed to broadcast message");
  //   }

  //   // Check if all peers have been found
  //   if (current_peer_count == ESPNOW_PEER_COUNT) {
  //     // Wait until all peers are ready
  //     if (check_all_peers_ready()) {
  //       Serial.println("All peers are ready");
  //       // Check which device has the highest priority
  //       master_decided = true;
  //       uint32_t highest_priority = check_highest_priority();
  //       if (highest_priority == self_priority) {
  //         device_is_master = true;
  //         Serial.println("This device is the master");
  //       } else {
  //         for (int i = 0; i < ESPNOW_PEER_COUNT; i++) {
  //           if (peers[i]->priority == highest_priority) {
  //             peers[i]->peer_is_master = true;
  //             master_peer = peers[i];
  //             Serial.printf("Peer " MACSTR " is the master with priority %lu\n", MAC2STR(peers[i]->addr()), highest_priority);
  //             break;
  //           }
  //         }
  //       }
  //       Serial.println("The master has been decided");
  //     } else {
  //       Serial.println("Waiting for all peers to be ready...");
  //     }
  //   }
  // } else {
  //   if (!device_is_master) {
  //     // Send a message to the master
  //     new_msg.count = sent_msg_count + 1;
  //     new_msg.data = random(10000);
  //     if (!master_peer->send_message((const uint8_t *)&new_msg, sizeof(new_msg))) {
  //       Serial.println("Failed to send message to the master");
  //     } else {
  //       Serial.printf("Sent message to the master. Count: %lu, Data: %lu\n", new_msg.count, new_msg.data);
  //       sent_msg_count++;
  //     }

  //     // Check if it is time to report to peers
  //     if (sent_msg_count % REPORT_INTERVAL == 0) {
  //       // Send a message to the peers
  //       for (auto &peer : peers) {
  //         if (!peer->peer_is_master) {
  //           if (!peer->send_message((const uint8_t *)&new_msg, sizeof(new_msg))) {
  //             Serial.printf("Failed to send message to peer " MACSTR "\n", MAC2STR(peer->addr()));
  //           } else {
  //             Serial.printf("Sent message \"%s\" to peer " MACSTR "\n", new_msg.str, MAC2STR(peer->addr()));
  //           }
  //         }
  //       }
  //     }
  //   } else {
  //     // Check if it is time to report to peers
  //     if (recv_msg_count % REPORT_INTERVAL == 0) {
  //       // Report average data to the peers
  //       uint32_t avg = calc_average();
  //       new_msg.data = avg;
  //       for (auto &peer : peers) {
  //         new_msg.count = sent_msg_count + 1;
  //         if (!peer->send_message((const uint8_t *)&new_msg, sizeof(new_msg))) {
  //           Serial.printf("Failed to send message to peer " MACSTR "\n", MAC2STR(peer->addr()));
  //         } else {
  //           Serial.printf(
  //             "Sent message to peer " MACSTR ". Recv: %lu, Sent: %lu, Avg: %lu\n", MAC2STR(peer->addr()), recv_msg_count, new_msg.count, new_msg.data);
  //           sent_msg_count++;
  //         }
  //       }
  //     }
  //   }
  // }

  delay(ESPNOW_SEND_INTERVAL_MS);
}


/* Helper functions */

// Function to reboot the device
void MyNetwork::fail_reboot() {
  Serial.println("Rebooting in 5 seconds...");
  delay(5000);
  ESP.restart();
}

// Function to calculate the average of the data received
uint32_t MyNetwork::calc_average() {
  uint32_t avg = 0;
  for (auto &d : last_data) {
    avg += d;
  }
  avg /= last_data.size();
  return avg;
}

// Function to check if all peers are ready
bool MyNetwork::check_all_peers_ready() {
  for (auto &peer : peers) {
    if (!peer->peer_ready) {
      return false;
    }
  }
  return true;
}

/* Callbacks */

// Callback called when a new peer is found
void MyNetwork::register_new_peer(const esp_now_recv_info_t *info, const uint8_t *data, int len, void *arg) {
  MyNetwork* network = static_cast<MyNetwork*>(arg);
  esp_now_data_t *msg = (esp_now_data_t *)data;

  if (current_peer_count < ESPNOW_PEER_COUNT) {
    Serial.printf("New peer found: " MACSTR "\n", MAC2STR(info->src_addr));
    ESP_NOW_Network_Peer *new_peer = new ESP_NOW_Network_Peer(info->src_addr);
    if (new_peer == nullptr || !new_peer->begin()) {
      Serial.println("Failed to create or register the new peer");
      delete new_peer;
      return;
    }
    peers.push_back(new_peer);
    current_peer_count++;
    if (current_peer_count == ESPNOW_PEER_COUNT) {
      Serial.println("All peers have been found");
      network->new_msg.ready = true;
    }
  }
}
