/******************************************************************************
 * OpenHD
 *
 * Licensed under the GNU General Public License (GPL) Version 3.
 *
 * This software is provided "as-is," without warranty of any kind, express or
 * implied, including but not limited to the warranties of merchantability,
 * fitness for a particular purpose, and non-infringement. For details, see the
 * full license in the LICENSE file provided with this source code.
 *
 * Non-Military Use Only:
 * This software and its associated components are explicitly intended for
 * civilian and non-military purposes. Use in any military or defense
 * applications is strictly prohibited unless explicitly and individually
 * licensed otherwise by the OpenHD Team.
 *
 * Contributors:
 * A full list of contributors can be found at the OpenHD GitHub repository:
 * https://github.com/OpenHD
 *
 * © OpenHD, All Rights Reserved.
 ******************************************************************************/
#ifndef OPENHD_OPENHD_OHD_COMMON_INC_OPENHD_CONFIG_H_
#define OPENHD_OPENHD_OHD_COMMON_INC_OPENHD_CONFIG_H_

#include <string>
#include <vector>
#include <memory>
#include <spdlog/logger.h>

namespace openhd {

// Default value for RPI Ethernet-only configuration
static constexpr auto RPI_ETHERNET_ONLY = "RPI_ETHERNET_ONLY";

// Configuration struct for managing OpenHD settings
struct Config {
  // WIFI Configuration
  bool WIFI_ENABLE_AUTODETECT = true;                  // Auto-detect WiFi cards
  std::vector<std::string> WIFI_WB_LINK_CARDS{};       // List of WB link cards
  std::string WIFI_WIFI_HOTSPOT_CARD;                  // Hotspot card
  bool WIFI_MONITOR_CARD_EMULATE = false;              // Emulate monitor mode
  bool WIFI_FORCE_NO_LINK_BUT_HOTSPOT = false;         // Force hotspot mode
  bool WIFI_LOCAL_NETWORK_ENABLE = false;              // Enable local network
  std::string WIFI_LOCAL_NETWORK_SSID;                 // SSID for local network
  std::string WIFI_LOCAL_NETWORK_PASSWORD;             // Password for local network

  // Networking Configuration
  std::string NW_ETHERNET_CARD = RPI_ETHERNET_ONLY;    // Ethernet card (default RPI)
  std::vector<std::string> NW_MANUAL_FORWARDING_IPS;   // Manual forwarding IPs
  bool NW_FORWARD_TO_LOCALHOST_58XX = false;           // Forward to localhost

  // Ethernet Link Configuration
  std::string GROUND_UNIT_IP = "";                     // Ground unit IP address
  std::string AIR_UNIT_IP = "";                        // Air unit IP address
  int VIDEO_PORT = 5000;                               // Video port
  int TELEMETRY_PORT = 5600;                           // Telemetry port

  // Microhard Ethernet Link Configuration
  bool DISABLE_MICROHARD_DETECTION = false;            // Disable detection
  bool FORCE_MICROHARD = false;                        // Force Microhard mode
  std::string MICROHARD_USERNAME = "admin";            // Username
  std::string MICROHARD_PASSWORD = "qwertz1";          // Password
  std::string MICROHARD_IP_AIR = "";                   // Air-side IP
  std::string MICROHARD_IP_GROUND = "";                // Ground-side IP
  std::string MICROHARD_IP_RANGE = "";                 // IP range
  int MICROHARD_VIDEO_PORT = 5910;                     // Video port
  int MICROHARD_TELEMETRY_PORT = 5920;                 // Telemetry port

  // General Configuration
  bool GEN_ENABLE_LAST_KNOWN_POSITION = false;         // Enable last known position
  int GEN_RF_METRICS_LEVEL = 0;                        // RF metrics level
  bool GEN_NO_QOPENHD_AUTOSTART = false;               // Disable auto-start
};

// Sets a custom configuration file path
void set_config_file(const std::string& config_file_path);

// Loads configuration from the specified or default file
Config load_config();

// Logs the configuration for debugging purposes
void debug_config(const Config& config);
void debug_config();

// Determines if Ethernet card management is manual
bool nw_ethernet_card_manual_active(const Config& config);

// Logger instance for OpenHD configuration
extern std::shared_ptr<spdlog::logger> m_console;

}  // namespace openhd

#endif  // OPENHD_CONFIG_H_
