#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include "Network/ESPNowNetwork.hpp"

namespace Networking {

// Uncomment for protocol debug
// #define PROTOCOL_DEBUG

#ifdef PROTOCOL_DEBUG
  #define PROTO_PRINT(...)    Serial.printf(__VA_ARGS__)
  #define PROTO_PRINTLN(msg)  Serial.println(msg)
#else
  #define PROTO_PRINT(...)    ((void)0)
  #define PROTO_PRINTLN(msg)  ((void)0)
#endif


// ################################################################
// ######  GAMEPROTOCOL CLASS DEFINITION  #########################
// ################################################################

class GameProtocol {
public:
  enum class MsgType : uint8_t { INVITE = 0, ACCEPT = 1, DATA = 2 };
  enum class State : uint8_t { IDLE = 0, WAITING = 1, CONNECTED = 2 };

  using ConnectedCallback = std::function<void(const uint8_t peerMac[6], int gameId, bool iAmInviter)>;

  struct DataPacket {
    uint8_t   mac[6];
    int    gameId;
    size_t    len;
    uint8_t   data[200];
  };

  static constexpr int DATA_BUFFER_SIZE = 16;
  static constexpr int MAX_GAME_NAME_LENGTH = 32;

  static GameProtocol& get() {
    static GameProtocol inst;
    return inst;
  }

  void reset();
  bool begin(uint8_t channel = 6);
  void invite(int id);
  void onConnected(ConnectedCallback cb);
  bool sendData(const void* ptr, size_t len, bool reliable = true);
  bool hasPacket() const;
  bool getPacket(DataPacket& pkt);
  void loop();

  bool isConnected() const;
  int getCurrentGameId() const;

  void getPeerMac(uint8_t mac[6]) const;

private:
  GameProtocol()
    : _state(State::IDLE),
      _currentGameId(-1),
      _bufHead(0),
      _bufTail(0),
      _lastInvite(0) {}

  void _sendInvite();
  void _sendAccept(const uint8_t mac[6], int gameId);
  void _onNetworkReceive(const uint8_t* mac, const uint8_t* data, size_t len);
  void _handleInvite(const uint8_t* mac, int gameId);
  void _handleAccept(const uint8_t* mac, int gameId);
  void _handleData(const uint8_t* mac, int gameId, const uint8_t* data, size_t len);

  ESPNowNetwork&    _network =  ESPNowNetwork::get();
  State             _state;
  int            _currentGameId;
  uint8_t           _peerMac[6];
  
  DataPacket        _dataBuffer[DATA_BUFFER_SIZE];
  volatile int      _bufHead;
  volatile int      _bufTail;
  
  ConnectedCallback _connectedCallback;
  unsigned long     _lastInvite;
  
  static constexpr unsigned long INVITE_INTERVAL = 1000;
};


// ################################################################
// ######  GAMEPROTOCOL CLASS IMPLEMENTATION  #####################
// ################################################################

void GameProtocol::reset() {
  _state = State::IDLE;
  _currentGameId = 0;
  memset(_peerMac, 0, 6);
  _bufHead = 0;
  _bufTail = 0;
  _connectedCallback = nullptr;
  _lastInvite = 0;
  
  PROTO_PRINTLN("Protocol: reset to initial state");
}

bool GameProtocol::begin(uint8_t channel) {
  if (!_network.begin(channel)) {
    return false;
  }
  
  _network.onReceive([this](const uint8_t* mac, const uint8_t* data, size_t len) {
    _onNetworkReceive(mac, data, len);
  });
  
  PROTO_PRINTLN("Protocol: initialized");
  return true;
}

void GameProtocol::invite(int id) {
  if (_state == State::CONNECTED) {
    PROTO_PRINTLN("Protocol: invite ignored, already connected");
    return;
  }
  
  _currentGameId = id;
  _state = State::WAITING;
  _lastInvite = 0;
  PROTO_PRINT("Protocol: inviting for game %d\n", id);
}

void GameProtocol::onConnected(ConnectedCallback cb) {
  _connectedCallback = cb;
}

bool GameProtocol::sendData(const void* ptr, size_t len, bool reliable) {
  if (_state != State::CONNECTED) {
    PROTO_PRINTLN("Protocol: sendData failed, not connected");
    return false;
  }
  
  uint8_t packet[250];
  packet[0] = uint8_t(MsgType::DATA);
  
  memcpy(packet + 1, &_currentGameId, sizeof(int));
  
  size_t headerSize = 1 + sizeof(int);
  size_t maxPayloadLen = min(len, sizeof(packet) - headerSize);
  memcpy(packet + headerSize, ptr, maxPayloadLen);
  
  auto result = reliable 
    ? _network.sendReliable(_peerMac, packet, headerSize + maxPayloadLen)
    : _network.sendUnreliable(_peerMac, packet, headerSize + maxPayloadLen);
    
  return result == ESPNowNetwork::SendResult::SUCCESS;
}

bool GameProtocol::hasPacket() const {
  return _bufHead != _bufTail;
}

bool GameProtocol::getPacket(DataPacket& pkt) {
  if (!hasPacket()) return false;
  pkt = _dataBuffer[_bufTail];
  _bufTail = (_bufTail + 1) % DATA_BUFFER_SIZE;
  return true;
}

void GameProtocol::loop() {
  _network.loop();

  if (_state == State::WAITING) {
    unsigned long now = millis();
    if (now - _lastInvite >= INVITE_INTERVAL) {
      _lastInvite = now;
      _sendInvite();
    }
  }
}

bool GameProtocol::isConnected() const {
  return _state == State::CONNECTED;
}

int GameProtocol::getCurrentGameId() const {
  return _currentGameId;
}

void GameProtocol::getPeerMac(uint8_t mac[6]) const {
  if (_state == State::CONNECTED) {
    memcpy(mac, _peerMac, 6);
  } else {
    memset(mac, 0, 6);
  }
}


void GameProtocol::_sendInvite() {
  uint8_t msg[5];
  msg[0] = uint8_t(MsgType::INVITE);
  memcpy(msg + 1, &_currentGameId, sizeof(int));
  
  _network.broadcast(msg, 5);
  PROTO_PRINT("Protocol: INVITE sent for game %d\n", _currentGameId);
}

// Send ACCEPT to specific peer
void GameProtocol::_sendAccept(const uint8_t mac[6], int gameId) {
  uint8_t msg[5];
  msg[0] = uint8_t(MsgType::ACCEPT);
  memcpy(msg + 1, &gameId, sizeof(int));
  
  _network.registerPeer(mac);
  _network.sendUnreliable(mac, msg, 5);
  PROTO_PRINT("Protocol: ACCEPT sent to " MACSTR "\n", MAC2STR(mac));
}


void GameProtocol::_onNetworkReceive(const uint8_t* mac, const uint8_t* data, size_t len) {
  if (len < 5) {
    PROTO_PRINTLN("Protocol: packet too short");
    return;
  }
  
  MsgType msgType = MsgType(data[0]);
  
  int gameId;
  memcpy(&gameId, data + 1, sizeof(int));
  
  PROTO_PRINT("Protocol: recv %u from " MACSTR " gameId=%d (state=%u)\n",
              uint8_t(msgType), MAC2STR(mac), gameId, uint8_t(_state));

  switch (msgType) {
    case MsgType::INVITE:
      _handleInvite(mac, gameId);
      break;
      
    case MsgType::ACCEPT:
      _handleAccept(mac, gameId);
      break;
      
    case MsgType::DATA:
      _handleData(mac, gameId, data + 5, len - 5);
      break;
  }
}

void GameProtocol::_handleInvite(const uint8_t* mac, int gameId) {
  if (_state != State::CONNECTED && _currentGameId == gameId) {
    if (_state == State::WAITING) {
      uint8_t myMac[6];
      WiFi.macAddress(myMac);
      
      bool shouldAccept = memcmp(myMac, mac, 6) < 0;
      
      if (shouldAccept) {
        PROTO_PRINTLN("Protocol: accepting INVITE (MAC comparison)");
        memcpy(_peerMac, mac, 6);
        _sendAccept(mac, gameId);
        _state = State::CONNECTED;
        
        if (_connectedCallback) {
          _connectedCallback(mac, gameId, false);
        }
      }
    } else {
      PROTO_PRINTLN("Protocol: accepting matching INVITE");
      memcpy(_peerMac, mac, 6);
      _sendAccept(mac, gameId);
      _state = State::CONNECTED;
      
      if (_connectedCallback) {
        _connectedCallback(mac, gameId, false);
      }
    }
  }
}

void GameProtocol::_handleAccept(const uint8_t* mac, int gameId) {
  if (_state == State::WAITING && _currentGameId == gameId) {
    PROTO_PRINTLN("Protocol: ACCEPT received, connected!");
    memcpy(_peerMac, mac, 6);
    _network.registerPeer(mac);
    _state = State::CONNECTED;
    
    if (_connectedCallback) {
      _connectedCallback(mac, gameId, true);
    }
  }
}

void GameProtocol::_handleData(const uint8_t* mac, int gameId, const uint8_t* data, size_t len) {
  if (_state != State::CONNECTED) {
    PROTO_PRINTLN("Protocol: DATA ignored, not connected");
    return;
  }
  
  int next = (_bufHead + 1) % DATA_BUFFER_SIZE;
  if (next != _bufTail) {
    DataPacket& pkt = _dataBuffer[_bufHead];
    memcpy(pkt.mac, mac, 6);
    pkt.gameId = gameId;
    pkt.len = min(len, sizeof(pkt.data));
    memcpy(pkt.data, data, pkt.len);
    _bufHead = next;
    PROTO_PRINT("Protocol: buffered DATA %u bytes\n", pkt.len);
  } else {
    PROTO_PRINTLN("Protocol: DATA buffer full, dropping");
  }
}

} // namespace Networking
