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
#include <esp_mac.h> // For the MAC2STR and MACSTR macros
#include <stdint.h>

#include "esp32-hal.h"

/* Definitions */

// Wi-Fi interface to be used by the ESP-NOW protocol
#define ESPNOW_WIFI_IFACE WIFI_IF_STA

// Channel to be used by the ESP-NOW protocol
#define ESPNOW_WIFI_CHANNEL 4

#define ESPNOW_SEND_INTERVAL_MS 5

#define ESPNOW_PEER_COUNT 5

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
  uint32_t count;
  uint32_t data;
  bool ready;
  char str[7];
} __attribute__((packed)) esp_now_data_t;

// Class that inherits from ESP_NOW_Peer and implement the _onReceive and
// _onSent methods. This class will be used to send messages to the peers. For
// more information about the ESP_NOW_Peer class, see the ESP_NOW_Peer class in
// the ESP32_NOW.h file.
class ESP_NOW_Network_Peer : public ESP_NOW_Peer {
public:
  ESP_NOW_Network_Peer(const uint8_t *mac_addr,
                       const uint8_t *lmk = (const uint8_t *)ESPNOW_EXAMPLE_LMK)
      : ESP_NOW_Peer(mac_addr, ESPNOW_WIFI_CHANNEL, ESPNOW_WIFI_IFACE, lmk) {}
  ~ESP_NOW_Network_Peer() {}

  bool begin();
  bool send_message(const uint8_t *data, size_t len);
  void onReceive(const uint8_t *data, size_t len, bool broadcast);
  void onSent(bool success);

public:
  bool peer_ready = false;

private:
};

class CustomNetworkManager {
public:
  CustomNetworkManager();
  ~CustomNetworkManager();

  void setup();
  void update();
  void onReceive(const uint8_t *data, size_t len, bool broadcast);
  void onSent(bool success);

private:
  void fail_reboot();
  uint32_t calc_average();
  bool check_all_peers_ready();
  // Callbacks
  static void register_new_peer(const esp_now_recv_info_t *info,
                                const uint8_t *data, int len, void *arg);

private:
  uint32_t version;
  ESP_NOW_Network_Peer
      broadcast_peer; // Register the broadcast peer (no encryption support for
                      // the broadcast address)
  esp_now_data_t new_msg; // Message that will be sent to the peers
  // TODO Add pool for in and out messages
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
