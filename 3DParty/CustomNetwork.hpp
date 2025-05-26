/*
  Networking
  3D PARTY
*/

#ifndef CUSTOM_NETWORK_HPP
#define CUSTOM_NETWORK_HPP

#include <ESP32_NOW.h>
#include <ESP32_NOW_Serial.h>
#include <WiFi.h>
#include <cstddef>
#include <cstdint>
#include <esp_mac.h> // For the MAC2STR and MACSTR macros
#include <stdint.h>
#include <vector>

#include "esp32-hal.h"
#include "enum.hpp"

/* Definitions */

// Wi-Fi interface to be used by the ESP-NOW protocol
#define ESPNOW_WIFI_IFACE WIFI_IF_STA

// Channel to be used by the ESP-NOW protocol
#define ESPNOW_WIFI_CHANNEL 4

#define ESPNOW_SEND_INTERVAL_MS 5

/*
    ESP-NOW uses the CCMP method, which is described in IEEE Std. 802.11-2012,
   to protect the vendor-specific action frame. The Wi-Fi device maintains a
   Primary Master Key (PMK) and several Local Master Keys (LMK). The lengths of
   both PMK and LMK need to be 16 bytes.

    PMK is used to encrypt LMK with the AES-128 algorithm. If PMK is not set, a
   default PMK will be used.

    LMK of the paired device is used to encrypt the vendor-specific action frame
   with the CCMP method. The maximum number of different LMKs is six. If the LMK
   of the paired device is not set, the vendor-specific action frame will not be
   encrypted.

    Encrypting multicast (broadcast address) vendor-specific action frame is not
   supported.

    PMK needs to be the same for all devices in the network. LMK only needs to
   be the same between paired devices.
*/

// Primary Master Key (PMK) and Local Master Key (LMK)
#define ESPNOW_EXAMPLE_PMK "pmk1234567890123"
#define ESPNOW_EXAMPLE_LMK "lmk1234567890123"

namespace CustomNetwork {

/* Structs */

// The following struct is used to send data to the peer device.
// We use the attribute "packed" to ensure that the struct is not padded (all
// data is contiguous in the memory and without gaps). The maximum size of the
// complete message is 250 bytes (ESP_NOW_MAX_DATA_LEN).
typedef struct {
  ServiceId service_id;
  uint32_t count;
  uint8_t payload_len;
  uint8_t payload[ESP_NOW_MAX_DATA_LEN]; // Payload of the message, can be up to 240 bytes
} __attribute__((packed)) esp_now_data_t;

typedef struct {
  ConnectionType connection_type; // Type of connection (TCP, UDP, etc.)
  esp_now_data_t packet; // Pointer to the packet data
} message_t;


// Class that inherits from ESP_NOW_Peer and implement the _onReceive and
// _onSent methods. This class will be used to send messages to the peers. For
// more information about the ESP_NOW_Peer class, see the ESP_NOW_Peer class in
// the ESP32_NOW.h file.
class ESP_NOW_Network_Peer : public ESP_NOW_Peer {
public:
  ESP_NOW_Network_Peer(const uint8_t *mac_addr, const uint8_t *lmk = (const uint8_t *)ESPNOW_EXAMPLE_LMK);
  ~ESP_NOW_Network_Peer() {}

  bool begin();
  bool sendPacket(esp_now_data_t packet);

private:
  // Callbacks
  void onReceive(const uint8_t *data, size_t len, bool broadcast);
  void onSent(bool success);

private:
  uint32_t recv_packet_count = 0;
  uint32_t sent_packet_count = 0;
  std::vector<uint32_t> last_data; // Vector that will store the last 5 data received
};

// CustomNetworkManager class
// This class will manage the ESP-NOW network, including the peers and the
// messages to be sent and received.

class CustomNetworkManager {
public:
  CustomNetworkManager();
  ~CustomNetworkManager() {}

  void setup();
  void update();
  void pushOutputMessage(uint8_t payload[], size_t payload_len, ServiceId service_id, ConnectionType connection_type);
  uint8_t *popInputMessage();

private:
  void applyOutputMessages();
  bool sendMessage(message_t message);
  void failReboot();
  // Callbacks
  static void registerNewPeer(const esp_now_recv_info_t *info, const uint8_t *data, int len, void *arg);

private:
  ESP_NOW_Network_Peer broadcast_peer;  // Register the broadcast peer (no encryption support for the broadcast address)
  ESP_NOW_Network_Peer *peer = nullptr;
  std::vector<message_t> _input_messages; // Vector that will store the messages received
  std::vector<message_t> _output_messages; // Vector that will store the messages to be sent
};

class LobbyProtocol {
public:
  LobbyProtocol(CustomNetworkManager &networkManager)
      : _networkManager(networkManager) {};
  ~LobbyProtocol();

  void setup();
  void update();
  void onReceive(const uint8_t *data, size_t len, bool broadcast);
  void onSent(bool success);

private:
    CustomNetworkManager &_networkManager;
};
} // namespace CustomNetwork

#endif // CUSTOM_NETWORK_HPP
