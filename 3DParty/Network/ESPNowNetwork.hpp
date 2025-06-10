#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <functional>

namespace Networking {

// Uncomment for network debug
// #define NETWORK_DEBUG

#ifdef NETWORK_DEBUG
  #define NET_PRINT(...)    Serial.printf(__VA_ARGS__)
  #define NET_PRINTLN(msg)  Serial.println(msg)
#else
  #define NET_PRINT(...)    ((void)0)
  #define NET_PRINTLN(msg)  ((void)0)
#endif


// ################################################################
// ######  ESPNOWNETWORK CLASS DEFINITION  ########################
// ################################################################

class ESPNowNetwork {
public:
  using ReceiveCallback = std::function<void(const uint8_t* mac, const uint8_t* data, size_t len)>;
  using SendCallback    = std::function<void(const uint8_t* mac, bool success)>;

  enum class SendResult { SUCCESS, QUEUE_FULL, NOT_INITIALIZED, PEER_NOT_FOUND };

  static constexpr int SEND_QUEUE_SIZE = 16;
  static constexpr int MAX_PACKET_SIZE = 246;

  static ESPNowNetwork& get();

  bool begin(uint8_t channel = 6);
  bool registerPeer(const uint8_t mac[6]);
  SendResult sendReliable(const uint8_t mac[6], const uint8_t* data, size_t len, unsigned timeoutMs = 500, unsigned maxAttempts = 6);
  SendResult sendUnreliable(const uint8_t mac[6], const uint8_t* data, size_t len);
  SendResult broadcast(const uint8_t* data, size_t len);
  void onReceive(ReceiveCallback cb);
  void onSendComplete(SendCallback cb);
  void loop();
private:
  ESPNowNetwork() 
    : _initialized(false), _channel(0), _peerCount(0),
      _queueHead(0), _queueTail(0), _sending(false) {}

  static constexpr int MAX_PEERS = 10;

  struct QueuedPacket {
    uint8_t       mac[6];
    uint8_t       data[MAX_PACKET_SIZE];
    size_t        len;
    bool          reliable;
    unsigned      timeoutMs;
    unsigned      maxAttempts;
    unsigned      attempts;
    unsigned long lastSent;
  };

  SendResult _queuePacket(const uint8_t mac[6], const uint8_t* data, size_t len, bool reliable, unsigned timeoutMs, unsigned maxAttempts);
  void _processSendQueue();
  void _dequeueCurrentPacket();

  static void _onRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len);
  static void _onSend(const uint8_t* mac_addr, esp_now_send_status_t status);

  bool            _initialized;
  uint8_t         _channel;
  uint8_t         _peers[MAX_PEERS][6];
  int             _peerCount;
  
  QueuedPacket    _sendQueue[SEND_QUEUE_SIZE];
  volatile int    _queueHead;
  volatile int    _queueTail;
  volatile bool   _sending;
  
  ReceiveCallback _recvCallback;
  SendCallback    _sendCallback;

  static constexpr uint8_t _bcast[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
};


// ################################################################
// ######  ESPNOWNETWORK CLASS IMPLEMENTATION  ####################
// ################################################################

ESPNowNetwork& ESPNowNetwork::get() {
    static ESPNowNetwork inst;
    return inst;
}

bool ESPNowNetwork::begin(uint8_t channel) {
    if (_initialized) return true;

    _channel = channel;
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.setChannel(_channel);

    NET_PRINT("Network: begin() MAC=%s ch=%u\n", WiFi.macAddress().c_str(), _channel);

    if (esp_now_init() != ESP_OK) {
        Serial.println("ERR: esp_now_init failed");
        return false;
    }

    esp_now_register_recv_cb(_onRecv);
    esp_now_register_send_cb(_onSend);

    esp_now_peer_info_t bc = {};
    memcpy(bc.peer_addr, _bcast, 6);
    bc.channel = _channel;
    bc.encrypt = false;
    if (esp_now_add_peer(&bc) != ESP_OK) {
        Serial.println("ERR: add broadcast peer failed");
        return false;
    }

    _initialized = true;
    NET_PRINTLN("Network: initialized successfully");
    return true;
}

bool ESPNowNetwork::registerPeer(const uint8_t mac[6]) {
    if (!_initialized) return false;

    for (int i = 0; i < _peerCount; i++) {
        if (memcmp(_peers[i], mac, 6) == 0) {
        return true;
        }
    }

    if (_peerCount >= MAX_PEERS) {
        NET_PRINTLN("Network: peer table full");
        return false;
    }

    esp_now_peer_info_t pi = {};
    memcpy(pi.peer_addr, mac, 6);
    pi.channel = _channel;
    pi.encrypt = false;

    if (esp_now_add_peer(&pi) == ESP_OK) {
        memcpy(_peers[_peerCount++], mac, 6);
        NET_PRINT("Network: peer registered " MACSTR "\n", MAC2STR(mac));
        return true;
    }
    return false;
}

ESPNowNetwork::SendResult ESPNowNetwork::sendReliable(const uint8_t mac[6], 
                        const uint8_t* data, 
                        size_t len,
                        unsigned timeoutMs,
                        unsigned maxAttempts) {
    return _queuePacket(mac, data, len, true, timeoutMs, maxAttempts);
}

ESPNowNetwork::SendResult ESPNowNetwork::sendUnreliable(const uint8_t mac[6], 
                        const uint8_t* data, 
                        size_t len) {
    return _queuePacket(mac, data, len, false, 0, 1);
}

ESPNowNetwork::SendResult ESPNowNetwork::broadcast(const uint8_t* data, size_t len) {
    return _queuePacket(_bcast, data, len, false, 0, 1);
}

void ESPNowNetwork::onReceive(ESPNowNetwork::ReceiveCallback cb) {
    _recvCallback = cb;
}

void ESPNowNetwork::onSendComplete(ESPNowNetwork::SendCallback cb) {
    _sendCallback = cb;
}

void ESPNowNetwork::loop() {
    if (!_initialized) return;
    _processSendQueue();
}

ESPNowNetwork::SendResult ESPNowNetwork::_queuePacket(const uint8_t mac[6],
                        const uint8_t* data,
                        size_t len,
                        bool reliable,
                        unsigned timeoutMs,
                        unsigned maxAttempts) {
    if (!_initialized) return SendResult::NOT_INITIALIZED;

    if (len > MAX_PACKET_SIZE) len = MAX_PACKET_SIZE;

    int next = (_queueHead + 1) % SEND_QUEUE_SIZE;
    if (next == _queueTail) {
        NET_PRINTLN("Network: send queue full");
        return SendResult::QUEUE_FULL;
    }

    QueuedPacket& pkt = _sendQueue[_queueHead];
    memcpy(pkt.mac, mac, 6);
    memcpy(pkt.data, data, len);
    pkt.len = len;
    pkt.reliable = reliable;
    pkt.timeoutMs = timeoutMs;
    pkt.maxAttempts = maxAttempts;
    pkt.attempts = 0;
    pkt.lastSent = 0;

    _queueHead = next;
    NET_PRINT("Network: queued %u bytes, reliable=%u\n", len, reliable);
    return SendResult::SUCCESS;
}

void ESPNowNetwork::_processSendQueue() {
    unsigned long now = millis();

    if (_sending && _queueTail != _queueHead) {
        QueuedPacket& pkt = _sendQueue[_queueTail];
        if (pkt.reliable && now - pkt.lastSent >= pkt.timeoutMs) {
        if (pkt.attempts < pkt.maxAttempts) {
            pkt.attempts++;
            NET_PRINT("Network: retry #%u\n", pkt.attempts);
            esp_now_send(pkt.mac, pkt.data, pkt.len);
            pkt.lastSent = now;
        } else {
            NET_PRINTLN("Network: packet timeout, dropping");
            _dequeueCurrentPacket();
        }
        }
        return;
    }

    if (!_sending && _queueTail != _queueHead) {
        QueuedPacket& pkt = _sendQueue[_queueTail];
        esp_now_send(pkt.mac, pkt.data, pkt.len);
        pkt.lastSent = now;
        pkt.attempts = 1;
        _sending = true;
        NET_PRINT("Network: sending %u bytes to " MACSTR "\n", 
                pkt.len, MAC2STR(pkt.mac));
    }
}

void ESPNowNetwork::_dequeueCurrentPacket() {
    if (_queueTail != _queueHead) {
        QueuedPacket& pkt = _sendQueue[_queueTail];
        if (_sendCallback) {
        _sendCallback(pkt.mac, false); // failed
        }
        _queueTail = (_queueTail + 1) % SEND_QUEUE_SIZE;
    }
    _sending = false;
}

void ESPNowNetwork::_onRecv(const esp_now_recv_info_t* info,
                    const uint8_t* data, int len) {
    auto& me = get();
    if (me._recvCallback) {
        me._recvCallback(info->src_addr, data, len);
    }
}

void ESPNowNetwork::_onSend(const uint8_t* mac_addr, esp_now_send_status_t status) {
    auto& me = get();
    if (!me._sending) return;

    if (me._queueTail != me._queueHead) {
        QueuedPacket& pkt = me._sendQueue[me._queueTail];
        
        if (status == ESP_NOW_SEND_SUCCESS || !pkt.reliable) {
            NET_PRINT("Network: packet completed (status=%u)\n", status);
            if (me._sendCallback) {
                me._sendCallback(pkt.mac, status == ESP_NOW_SEND_SUCCESS);
            }
            me._queueTail = (me._queueTail + 1) % SEND_QUEUE_SIZE;
            me._sending = false;
        }
    }
}

} // namespace Networking
