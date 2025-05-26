/*
  Enums for 3D Party
  3D PARTY
*/

#ifndef ENUM_HPP
#define ENUM_HPP

/**
 * @brief Enum identifying service origin
 *
 * This enum is used to identify the service origin of the message.
 */
enum ServiceId {
  NONE = 0,
  TEST = 1,
};

enum ConnectionType {
  TCP = 1,
  UDP = 2,
};

#endif // ENUM_HPP
