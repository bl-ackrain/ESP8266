
#pragma once

#include <mbed.h>
#include <string>
#include <cstdint>
#include <cstddef>


enum class Commands: std::uint16_t
{
    nop=1,
    restart,
    checkVersion,
    getVersion,
    getVersionString,
    setBaudRate,
    eraseConfig,
    // Wifi
    setWifiMode,
    getWifiMode,
    joinAP,
    getStatus,
    leaveAP,
    getSSID,
    getRSSI,
    getLocalIP,
    getGatewayIP,
    getSubnetMask,
    getMac,
    setStationIP,
    scanNetworks,
    scanComplete,
    getNetworkInfo,
    //wifi softAP
    setSoftAPConfig,
    getSoftAPConfig,
    setSoftAPIP,
    getSoftAPIP,
    softAPdisconnect,
    softAPgetStationNum,
    getSoftAPClient,
    //TCP client
    createTCP,
    sendTCP,
    availableTCP,
    readTCP,
    closeTCP,
    isConnectedTCP,
    //UDP
    createUDP,
    sendUDP,
    listenUDP,
    availableUDP,
    closeUDP,
    readUDP,
    getRemoteInfoUDP,
    //http client
    createHTTP,
    sendGetHTTP,
    getStringHTTP,
    readDataHTTP,
    getSizeHTTP,
    closeHTTP,
    setFingerPrintHTTP,
    setInSecureHTTP,
    addHeaderHTTP,
    getResponseHeaderCountHTTP,
    getResponseHeaderHTTP,
    sendPostHttp,
    // ESP-NOW https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/network/esp_now.html
    espNowInit,
    espNowAddPeer,
    espNowRemovePeer,
    espNowSend,
    espNowReceive,
    espNowDeInit,
};

enum class Response: std::uint16_t
{
	Ok = 1,
	Error,
	String,
	Data,
};

enum class WiFiMode 
{
    Off = 0,
	Station = 1,
	AcessPoint = 2,
	AcessPointStation = (AcessPoint | Station),
};

enum class WifiStatus : std::uint8_t
{
	Idle = 0,
	NoSSIDAvailable = 1,
	ScanComplete = 2,
	Connected = 3,
	ConnectFailed = 4,
	ConnectionLost = 5,
	Disconnected = 6,
	NoShield = 255,
};

enum class EncryptionType : std::uint8_t
{
    WEP  = 5,
    WPA_PSK = 2,
    WPA2_PSK = 4,
    None = 7,
    WPA_WPA2_PSK = 8
};

struct NetworkInfo
{
    char ssid[33];
    EncryptionType encryptionType;
    std::int32_t rssi;
    std::uint8_t bssid[6];
    std::int32_t channel;
    bool isHidden;
};

struct EspNowReceiveInfo
{
    std::uint8_t Sender[6];
    std::uint8_t Data[250];
    std::size_t Size;
};

/// @brief
/// class ESP8266
///
class ESP8266
{
private:
	Serial* uart;

	DigitalOut pinEnable;
	DigitalOut pinReset;
	DigitalOut pinProg;

private:
    std::string receiveString(const std::uint32_t timeout);
    bool receiveOk(const std::uint32_t timeout);
    Response getResponse(const std::uint32_t timeout);

    void sendData(const std::uint8_t* data, const std::uint16_t size);
    std::uint16_t read16(void);
    void write16(const std::uint16_t value);
    void sendCommand(const Commands command);
    void sendString(const std::string &String);
    std::size_t readBuffer(std::uint8_t* buffer, const std::size_t bufferSize, const std::uint32_t timeout);

 public:
    ESP8266(std::uint32_t baud = 230400);

    
    /// @brief
    /// Initialisation of ESP8266
    ///
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool begin(void);
    
    /// @brief
    /// Verify ESP8266 whether alive or not. 
    /// 
    /// @retval true - alive.
    /// @retval false - dead.
    ///
    bool isPresent(void);
    
    /// @brief
    /// Soft restart ESP8266. 
    ///
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool restart(void);
    
    /// @brief
    /// Check if the version match the installed one.
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool checkVersion(const std::string &version);

    
    /// @brief
    /// Get the version of the Lib. 
    /// 
    /// @return the version. 
    ///
    std::uint16_t getVersion(void);
    
    /// @brief
    /// Get the version of the lib in string format. 
    /// 
    /// @return the string of version. 
    ///
    std::string getVersionString(void);
    
    /// @brief
    /// Set the buad rate to communicate with ESP8266.
    ///
    /// @param baud - the buad rate to communicate with ESP8266(default:230400). 
    ///
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool setBaudRate(const std::uint32_t baud=230400);
    
    /// @brief
    /// Erase the internal config of ESP8266.
    ///
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool eraseConfig(void);

    //
    // WiFi
    //

    /// @brief
    /// Set wifi mode 
    /// 
    /// @param mode - the wifi mode
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool setWifiMode(const WiFiMode mode);
    
    /// @brief
    /// Get wifi mode 
    /// 
    /// @return wifi mode
    ///
    WiFiMode getWifiMode(void);
    
    /// @brief
    /// Join in AP. 
    ///
    /// @param ssid - SSID of AP to join in. 
    /// @param password - Password of AP to join in. 
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool joinAP(const std::string &ssid, const std::string &password);
    
    /// @brief
    /// Get Connection status.
    /// 
    /// @return one of the value defined in WifiStatus
    ///
    WifiStatus getStatus();
    
    /// @brief
    /// Leave AP joined before. 
    ///
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool leaveAP(void);
    
    /// @brief
    /// Get the SSID of Access Point ESP8266 is connected to.
    /// 
    /// @return the SSID of Access Point.
    ///
    std::string getSSID(void);
    
    /// @brief
    /// Get the RSSI of Access Point ESP8266 is connected to.
    /// 
    /// @return the RSSI of Access Point.
    ///
    std::int32_t getRSSI(void);
    
    /// @brief
    /// Get the IP address of ESP8266. 
    ///
    /// @return the IP list. 
    ///
    std::string getLocalIP(void);
    
    /// @brief
    /// Get the Gateway address of Access Point ESP8266 is connected to.
    /// 
    /// @return the Gateway address of Access Point.
    ///
    std::string getGatewayIP(void);
    
    /// @brief
    /// Get the Subnet Mask of Access Point ESP8266 is connected to.
    /// 
    /// @return the Subnet Mask of Access Point.
    ///
    std::string getSubnetMask(void);
    
    /// @brief
    /// Get the Mac address of Access Point ESP8266 is connected to.
    /// 
    /// @return the Mac address of Access Point.
    ///
    std::string getMac(void);
    
    /// @brief
    /// Change IP configuration settings disabling the dhcp client
    /// 
    /// @param local_ip - static ip configuration
    /// @param gateway - static gateway configuration
    /// @param subnet - static Subnet mask
    /// @param dns1 - static DNS server 1
    /// @param dns2 - static DNS server 2
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool setStationIP(const std::string &local_ip, const std::string &gateway, const std::string &subnet, const std::string &dns1, const std::string &dns2);
    
    /// @brief
    /// Start Scaning available APs.
    /// 
    /// @param async - run in async mode
    /// @param show_hidden - show hidden networks
    /// @param channel - scan only this channel (0 for all channels)
    /// @param ssid - scan for only this ssid (NULL for all ssid's)
    /// 
    /// @retval true - success.
    /// @retval false - failure.
   ///
    bool scanNetworks(const bool async=true, const bool show_hidden=false, const std::uint8_t channel=0, const std::string &ssid="");
    
    /// @brief
    /// called to get the scan state in Async mode
    /// 
    /// @return scan result or status
    /// @retval > 0 numbre of networks found
    /// @retval -1 if scan in progress
    ///
    std::int16_t scanComplete(void);
    
    /// @brief
    /// Return the Network info discovered during the network scan.
    /// @param id - specify from which network item want to get the information
    /// @param info - a  refrence to NetworkInfo
    /// 
    /// @return all the network infos.
    ///
    
    bool getNetworkInfo(const std::uint16_t id, NetworkInfo &info);
    
    //
    // Wifi SoftAccesPoint
    //  
    
    /// @brief
    /// Set up an access point
    /// 
    /// @param ssid - The SSID (max 31 char).
    /// @param passphrase - For WPA2 min 8 char, (max 63 char).
    /// @param channel - WiFi channel number, 1 - 13.
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool setSoftAPConfig(const std::string &ssid, const std::string &passphrase="", const std::uint16_t channel=1);
    
    /// @brief
    /// Get the configured softAP SSID name and softAP PSK or PASSWORD.
    /// 
    /// @param ssid - a refernce to the SSID string.
    /// @param passphrase - a refernce password string.
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool getSoftAPConfig(std::string &ssid, std::string &passphrase);
    
    /// @brief
    /// Configure access point
    /// 
    /// @param local_ip - access point IP
    /// @param gateway - gateway IP ("0.0.0.0" to disable)
    /// @param subnet - subnet mask
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool setSoftAPIP(const std::string &local_ip, const std::string &gateway, const std::string &subnet);
    
    /// @brief
    /// Get the softAP interface IP address and MAC address.
    /// 
    /// @param ip_address - a refernce to AP IP string
    /// @param mac - a refernce to AP mac string
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool getSoftAPIP(std::string &ip_address, std::string &mac);
    
    /// @brief
    /// Disconnect from the network (close AP)
    /// 
    /// @param wifioff - disable mode?
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool softAPdisconnect(const bool wifioff=false);
    
    /// @brief
    /// Get the count of the Station / client that are connected to the softAP interface
    /// 
    /// @return Stations count
    ///
    std::uint16_t softAPgetStationNum(void);
    
    /// @brief
    /// Get the softAP connected client IP address and MAC address.
    /// 
    /// @param id - specify from which client want to get the information
    /// @param ip_address - a refernce to AP IP string
    /// @param mac - a refernce to AP mac string
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool getSoftAPClient(const std::uint16_t id, std::string &ip_address, std::string &mac);
    
    //
    // TCP
    //
 
    /// @brief
    /// Create TCP connection. 
    /// 
    /// @param id - the identifier of this TCP(available value: 0 - 4). 
    /// @param address - the IP or domain name of the target host. 
    /// @param port - the port number of the target host. 
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool createTCP(const std::uint8_t id, const std::string &address, const std::uint16_t port);
    
    /// @brief
    /// Close TCP connection. 
    /// 
    /// @param id - the identifier of this TCP(available value: 0 - 4). 
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool closeTCP(const std::uint8_t id=0);
    
    /// @brief
    /// Check if data available to receive. 
    /// 
    /// @param id - the identifier of this TCP(available value: 0 - 4). 
    /// 
    /// @retval true - data available.
    /// @retval false - data not available.
    ///
    bool availableTCP(const std::uint8_t id=0);

    /// @brief
    /// Send data based on one of TCP. 
    /// 
    /// @param id - the identifier of this TCP(available value: 0 - 4). 
    /// @param buffer - the buffer of data to send. 
    /// @param size - the length of data to send. 
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool sendTCP(const std::uint8_t id, const std::uint8_t* buffer, const std::uint16_t size);
    

    
    /// @brief
    /// Read data from one of TCP. 
    ///
    /// @param id - the identifier of this TCP(available value: 0 - 4). 
    /// @param buffer - the buffer for storing data. 
    /// @param buffer_size - the length of the buffer. 
    /// @param timeout - the time waiting data. 
    /// 
    /// @return the length of data received actually. 
    ///
    std::uint16_t readTCP(const std::uint8_t id, std::uint8_t* buffer, const std::uint16_t buffer_size, const std::uint32_t timeout = 1000);
   
   
    /// @brief
    /// Check if Tcp client connected. 
    /// 
    /// @param id - the identifier of this TCP(available value: 0 - 4).
    /// 
    /// @retval true - connected.
    /// @retval false - not connected.
    ///
    bool isConnectedTCP(const std::uint8_t id=0);
   
    //
    // UDP
    //

    /// @brief
    /// Create UDP packet.
    /// 
    /// @param id - the identifier of this UDP(available value: 0 - 4). 
    /// @param address - the IP or domain name of the target host. 
    /// @param port - the port number of the target host. 
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool createUDP(const std::uint8_t id, const std::string &address, const std::uint16_t port);
    
    /// @brief
    /// Close UDP server or packet. 
    /// 
    /// @param id - the identifier of this UDP(available value: 0 - 4). 
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool closeUDP(const std::uint8_t id=0);
    
    /// @brief
    /// Send a UDP Packet. 
    /// 
    /// @param id - the identifier of this UDP(available value: 0 - 4). 
    /// @param buffer - the buffer of data to send. 
    /// @param size - the length of data to send. 
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool sendUDP(const std::uint8_t id, const std::uint8_t* buffer, const std::uint16_t size);
    
    /// @brief
    /// Read data from Udp Packet. 
    ///
    /// @param id - the identifier of this UDP(available value: 0 - 4). 
    /// @param buffer - the buffer for storing data. 
    /// @param buffer_size - the length of the buffer. 
    /// @param timeout - the time waiting data. 
    /// 
    /// @return the length of data received actually. 
    ///
    std::uint16_t readUDP(const std::uint8_t id, std::uint8_t* buffer, const std::uint16_t buffer_size, const std::uint32_t timeout = 1000);
    
    /// @brief
    /// Check if a packet available to read. 
    /// 
    /// @param id - the identifier of this UDP(available value: 0 - 4). 
    /// 
    /// @retval true - data available.
    /// @retval false - data not available.
    ///
    bool availableUDP(const std::uint8_t id=0);
    
    
    /// @brief
    /// Check if a packet available to read. 
    /// 
    /// @param id - the identifier of this UDP(available value: 0 - 4).
    /// @param port - the port number to listen(default: 333)
    /// 
    /// @retval true - data available.
    /// @retval false - data not available.
    ///
    bool listenUDP(std::uint8_t id=0, std::uint16_t port=333);
   
    /// @brief
    /// Return the address and port of udp packet sender.
    ///
    /// @param id - the identifier of this UDP(available value: 0 - 4).
    /// @param adress -  a refrence to adress string
    /// @param port - a refrence to port
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool getRemoteInfoUDP(const std::uint8_t id, std::string &address, std::uint16_t &port);
    
    //
    // HTTP
    //
 
    /// @brief
    /// Create HTTP.
    /// 
    /// @param host - the IP or domain name of the target host. 
    /// @param port - the port number of the target host. 
    /// @param uri - the uri
    /// @param is_https    https or http
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool createHTTP(const std::string &host, const std::uint16_t port=80, const std::string &uri="/", const bool is_https=false);
 
    /// @brief
    /// Send Http GET request. 
    /// 
    /// @param timeout - the time waiting data. 
    /// 
    /// @return error codeon failure or http code on success.
    ///
    std::int32_t sendGetHTTP(const std::uint32_t timeout=5000);
    
    /// @brief
    /// Return the content of the http response as a String.
    ///
    /// @return String.
    ///
    std::string getStringHTTP(void);
    
    /// @brief
    /// Read the content of the http response.
    /// 
    /// @param buffer - the buffer for storing data. 
    /// @param buffer_size - the length of the buffer. 
    /// @param timeout - the time waiting data.
    /// 
    /// @return the length of data received actually.
    ///
    std::uint32_t readDataHTTP(std::uint8_t* buffer, const std::uint16_t buffer_size, const std::uint32_t timeout=1000);
    
    /// @brief
    /// Get size of message body
    /// 
    /// @return -1 if no info or > 0 when Content-Length is set by server
    ///
    std::uint32_t getSizeHTTP(void);
    
    /// @brief
    /// Close HTTP. 
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool closeHTTP(void);
    
    /// @brief
    /// HTTPS SHA1 fingerprint of certificate to check.
    /// 
    /// @param fingerprint - SHA1 fingerprint of certificate.
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool setFingerPrintHTTP(const std::uint8_t fingerprint[]);
    
    /// @brief
    /// HTTPS Don't validate the chain, just accept whatever is given.  VERY INSECURE!
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool setInSecureHTTP(void);
    
    /// @brief
    /// Add a Header to the request.
    ///
    /// @param name - header name
    /// @param value - header value 
    ///
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool addHeaderHTTP(const std::string &name, const std::string &value);
    
    /// @brief
    /// Get response header count.
    ///
    /// @return header count.
    ///
    std::size_t getResponseHeaderCountHTTP(void);
    
    /// @brief
    /// Get response header by number.
    ///
    /// @param name - header name
    /// @param value - header value
    ///
    /// @return header count.
    ///
    bool getResponseHeaderHTTP(std::size_t id, std::string &name, std::string &value);
    
    /// @brief
    /// sends a post request to the server
    ///
    /// @param payload - payload buffer
    /// @param size -  payload size
    ///
    /// @return http code
    ///
    std::int32_t sendPostHttp(const std::uint8_t* payload, std::uint16_t size, std::uint32_t timeout=3000);
    
    //
    // ESP_NOW
    //
    
    /// @brief
    /// Initialize ESP-NOW.
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool espNowInit(void);
    
    /// @brief
    /// Add a peer or change peer channel.
    ///
    /// @param mac - peer MAC address
    /// @param - channel peer channel, 0 for current channel
    ///
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool espNowAddPeer(const std::string &mac, std::uint8_t channel=0);
    
    /// @brief 
    /// Remove a peer.
    ///
    /// @param mac -  peer MAC address
    ///
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool espNowRemovePeer(const std::string &mac);
    
    /// @brief
    /// Send a message via ESP-NOW.
    ///
    /// @param mac - destination MAC address, empty string for all peers
    /// @param buffer - payload
    /// @param size - payload size, must not exceed 250 bytes
    ///
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool espNowSend(const std::string &mac, std::uint8_t* buffer, const std::size_t size);
    
    /// @brief
    /// Receive a message via ESP-NOW.
    ///
    /// @param info - a refrence to EspNowReceiveInfo to receive infos
    ///
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool espNowReceive(EspNowReceiveInfo &info);
    
    /// @brief
    /// Deinitialize ESP-NOW.
    /// 
    /// @retval true - success.
    /// @retval false - failure.
    ///
    bool espNowDeInit();
  
};
