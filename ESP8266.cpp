///
/// @file ESP8266.cpp
/// @brief The implementation of class ESP8266 for Pokitto plus. 
/// @author bl_ackrain 
/// @date 2019
///
 
#include "ESP8266.h"
#include <Pokitto.h>


ESP8266::ESP8266( std::uint32_t baud)
: pinEnable(P0_21), pinReset(P0_20), pinProg(P1_1)
{
    this->uart=new Serial(USBTX, USBRX);
    this->uart->baud(baud);
}

bool ESP8266::begin(void)
{
    pinEnable = 1;
	pinProg = 1;
	pinReset = 1;
	
	return true;
}

bool ESP8266::isPresent(void)
{
	this->sendCommand(Commands::nop);
	return this->receiveOk(100);
}

bool ESP8266::restart(void)
{
	this->sendCommand(Commands::restart);
	return this->receiveOk(100);
}

bool ESP8266::checkVersion(const std::string &version)
{
    this->sendCommand(Commands::checkVersion);
    this->sendString(version);
    
    return this->receiveOk(100);
}

std::uint16_t ESP8266::getVersion(void)
{
    this->sendCommand(Commands::getVersion);

	if(this->getResponse(100) == Response::Data)
		if(this->read16() == 2)
			return this->read16();

	return 0;
}

std::string ESP8266::getVersionString(void)
{
	this->sendCommand(Commands::getVersionString);
	return this->receiveString(200);
}

bool ESP8266::setBaudRate(const std::uint32_t baud)
{
	this->sendCommand(Commands::setBaudRate);
    this->sendData(reinterpret_cast<const std::uint8_t*>(&baud), sizeof(baud));
    this->receiveOk(1000);
    this->uart->baud(baud);
    
    return this->receiveOk(1000);
}

bool ESP8266::eraseConfig(void)
{
    this->sendCommand(Commands::eraseConfig);
    return this->receiveOk(2000);
}

//
// WiFi
//

bool ESP8266::setWifiMode(const WiFiMode mode)
{
    this->sendCommand(Commands::setWifiMode);
    this->write16(static_cast<std::uint16_t>(mode));
    return this->receiveOk(200);
}

WiFiMode ESP8266::getWifiMode()
{
    this->sendCommand(Commands::getWifiMode);
    if(this->getResponse(200)==Response::Data)
    {
        if(this->read16()==2)
            return static_cast<WiFiMode>(this->read16());
    }
    return WiFiMode::Off;
}

bool ESP8266::joinAP(const std::string &ssid, const std::string &passwod)
{
    this->sendCommand(Commands::joinAP);

    this->sendString(ssid);
    this->sendString(passwod);

    return receiveOk(1000);
}

WifiStatus ESP8266::getStatus(void)
{
    this->sendCommand(Commands::getStatus);
    if(this->getResponse(100)==Response::Data)
    {
        if(this->read16()==2)
            return static_cast<WifiStatus>(this->read16());
    }
    return WifiStatus::Idle;
}

bool ESP8266::leaveAP(void)
{
    this->sendCommand(Commands::leaveAP);
    return this->receiveOk(1000);
}

std::string ESP8266::getSSID(void)
{
    this->sendCommand(Commands::getLocalIP);
    return this->receiveString(200);
}

std::int32_t ESP8266::getRSSI(void)
{
    this->sendCommand(Commands::getRSSI);

    if(this->getResponse(200) == Response::Data)
		if(this->read16() == 4)
		{
			const std::uint32_t low = this->read16();
			const std::uint32_t high = this->read16();
			return ((high << 16) | (low << 0));
		}

    return 0;
}

std::string ESP8266::getLocalIP(void)
{
    this->sendCommand(Commands::getLocalIP);
    return this->receiveString(200);
}

std::string ESP8266::getGatewayIP(void)
{
    this->sendCommand(Commands::getGatewayIP);
    return this->receiveString(200);
}

std::string ESP8266::getSubnetMask(void)
{
    this->sendCommand(Commands::getSubnetMask);
    return this->receiveString(200);
}

std::string ESP8266::getMac(void)
{
    this->sendCommand(Commands::getMac);
    return this->receiveString(200);
}

bool ESP8266::setStationIP(const std::string &local_ip, const std::string &gateway, const std::string &subnet, const std::string &dns1, const std::string &dns2)
{
    this->sendCommand(Commands::setStationIP);

    this->sendString(local_ip);
    this->sendString(gateway);
    this->sendString(subnet);
    this->sendString(dns1);
    this->sendString(dns2);
    
    return this->receiveOk(1000);
}

bool ESP8266::scanNetworks(const bool async, const bool show_hidden, const std::uint8_t channel, const std::string &ssid)
{
    this->sendCommand(Commands::scanNetworks);
    this->uart->putc(async?1:0);
    this->uart->putc(show_hidden?1:0);
    this->uart->putc(channel);

    this->sendString(ssid);

    return this->receiveOk(5000);
}


std::int16_t ESP8266::scanComplete(void)
{
    this->sendCommand(Commands::scanComplete);
    if(this->getResponse(200)!=Response::Data)
        return -1;

    if(this->read16()==2)
        return this->read16();
    
    return -1;
}

bool ESP8266::getNetworkInfo(const std::uint16_t id, NetworkInfo &info)
{
    this->sendCommand(Commands::getNetworkInfo);
    this->write16(id);
    
    if(this->getResponse(300)!=Response::Data)
        return false;

    const std::uint16_t size=this->read16();
    if(size!=sizeof(info))
        return false;
        
    std::uint8_t* infoBytes = reinterpret_cast<std::uint8_t*>(&info);
    for(std::size_t i = 0; i < sizeof(info); i++)
        infoBytes[i] = this->uart->getc();
    
    return true;
}

//
// Wifi SoftAccessPoint
//

bool ESP8266::setSoftAPConfig(const std::string &ssid, const std::string &passphrase, const std::uint16_t channel)
{
    this->sendCommand(Commands::setSoftAPConfig);
    this->write16(channel);
    this->sendString(ssid);
    
    this->sendString(passphrase);
    
    return this->receiveOk(2000);
}

bool ESP8266::getSoftAPConfig(std::string &ssid, std::string &passphrase)
{
    this->sendCommand(Commands::getSoftAPConfig);
    
    ssid=this->receiveString(200);
    passphrase=this->receiveString(200);
    return true;
}

bool ESP8266::setSoftAPIP(const std::string &local_ip, const std::string &gateway, const std::string &subnet)
{
    this->sendCommand(Commands::setSoftAPIP);

    this->sendString(local_ip);
    this->sendString(gateway);
    this->sendString(subnet);
    
    return this->receiveOk(5000);
}


bool ESP8266::getSoftAPIP(std::string &ip_address, std::string &mac)
{
    this->sendCommand(Commands::getSoftAPIP);
    
    ip_address=this->receiveString(200);
    mac=this->receiveString(200);
    return true;
}

bool ESP8266::softAPdisconnect(const bool wifioff)
{
    this->sendCommand(Commands::softAPdisconnect);
    this->write16(wifioff?1:0);
    
    return this->receiveOk(500);
}

std::uint16_t ESP8266::softAPgetStationNum(void)
{
    this->sendCommand(Commands::softAPgetStationNum);
    if(this->getResponse(200)!=Response::Data)
        return 0;

    if(this->read16()==2)
        return this->read16();
    
    return 0;
}

bool ESP8266::getSoftAPClient(const std::uint16_t id, std::string &ip_address, std::string &mac)
{
    this->sendCommand(Commands::getSoftAPClient);
    this->write16(id);
    
    ip_address = this->receiveString(200);
    mac = this->receiveString(200);
    return (ip_address.length()!=0);
}

// 
//TCP
//

bool ESP8266::createTCP(const std::uint8_t id, const std::string &address, const std::uint16_t port)
{
    this->sendCommand(Commands::createTCP);
    this->uart->putc(id);
    this->write16(port);
    this->sendString(address);
    
    return this->receiveOk(3000);
}

bool ESP8266::closeTCP(const std::uint8_t id)
{
    this->sendCommand(Commands::closeTCP);
    this->uart->putc(id);

    return this->receiveOk(1000);
}

bool ESP8266::sendTCP(const std::uint8_t id, const std::uint8_t* buffer, const std::uint16_t size)
{

    this->sendCommand(Commands::sendTCP);
    this->uart->putc(id);
    this->write16(size);
    
    for(std::size_t i=0; i<size;i++)
        this->uart->putc(buffer[i]);
    return this->receiveOk(3000);
}

std::uint16_t ESP8266::readTCP(const std::uint8_t id, std::uint8_t* buffer, const std::uint16_t buffer_size, const std::uint32_t timeout)
{
    this->sendCommand(Commands::readTCP);
    this->uart->putc(id);
    
    if(this->getResponse(300)!=Response::Data)
        return 0;

    return this->readBuffer(buffer, buffer_size, timeout);
}

bool ESP8266::availableTCP(const std::uint8_t id)
{
    this->sendCommand(Commands::availableTCP);
    this->uart->putc(id);

    return this->receiveOk(1000);
}

//
// UDP
//

bool ESP8266::createUDP(const std::uint8_t id, const std::string &address, const std::uint16_t port)
{
    this->sendCommand(Commands::createUDP);
    this->uart->putc(id);
    this->write16(port);
    this->sendString(address);
    
    return this->receiveOk(3000);
}

bool ESP8266::closeUDP(const std::uint8_t id)
{
    this->sendCommand(Commands::closeUDP);
    this->uart->putc(id);

    return this->receiveOk(500);
}

bool ESP8266::sendUDP(const std::uint8_t id, const std::uint8_t* buffer, const std::uint16_t size)
{
    this->sendCommand(Commands::sendUDP);
    this->uart->putc(id);
    this->write16(size);
    
    for(std::size_t i=0; i<size;i++)
        this->uart->putc(buffer[i]);
    return this->receiveOk(1000);
}

std::uint16_t ESP8266::readUDP(const std::uint8_t id, std::uint8_t* buffer, const std::uint16_t buffer_size, const std::uint32_t timeout)
{
    this->sendCommand(Commands::readUDP);
    this->uart->putc(id);
    
    if(this->getResponse(300)!=Response::Data)
        return 0;

    return this->readBuffer(buffer, buffer_size, timeout);
}

bool ESP8266::availableUDP(const std::uint8_t id)
{
    this->sendCommand(Commands::availableUDP);
    this->uart->putc(id);

    return this->receiveOk(20);
}

bool ESP8266::listenUDP(const std::uint8_t id, const std::uint16_t port)
{
    this->sendCommand(Commands::listenUDP);
    this->uart->putc(id);
    this->write16(port);
    return this->receiveOk(2000);
}

bool ESP8266::getRemoteInfoUDP(const std::uint8_t id, std::string &address, std::uint16_t &port)
{
    this->sendCommand(Commands::getRemoteInfoUDP);
    this->uart->putc(id);
    
    if(this->getResponse(300)!=Response::Data)
        return false;

    std::uint16_t size=this->read16();
    if(size==2)
        port=this->read16();
    else
        return false;
        
    address=this->receiveString(500);
    return(address.length()!=0);
    
}

//
// HTTP
//

bool ESP8266::createHTTP(const std::string &host, const std::uint16_t port, const std::string &uri, const bool is_https)
{
    this->sendCommand(Commands::createHTTP);

    this->write16(is_https?1:0);
    this->write16(port);
    this->sendString(host);
    this->sendString(uri);
    
    return this->receiveOk(3000);
}

std::int32_t ESP8266::sendGetHTTP(const std::uint32_t timeout)
{
    this->sendCommand(Commands::sendGetHTTP); 
 
    if(this->getResponse(timeout)!=Response::Data)
        return -1;
    if(this->read16()==4)
    {
        std::uint16_t low=this->read16();
        return ((this->read16()<<16)|low);
    }
    return -1;
}

std::string ESP8266::getStringHTTP(void)
{
    this->sendCommand(Commands::getStringHTTP);
    return this->receiveString(2000);
}

std::uint32_t ESP8266::readDataHTTP(std::uint8_t* buffer, const std::uint16_t buffer_size, const std::uint32_t timeout)
{
    this->sendCommand(Commands::readDataHTTP);
    
    if(this->getResponse(300)!=Response::Data)
        return 0;

    return this->readBuffer(buffer, buffer_size, timeout);
}

std::uint32_t ESP8266::getSizeHTTP(void)
{
    this->sendCommand(Commands::getSizeHTTP); 
    
    if(this->getResponse(300)!=Response::Data)
        return 0;
    
    if(this->read16()==4)
    {
        std::uint16_t low=this->read16();
        return((this->read16()<<16)|low);
    }
    
    return 0;
}

bool ESP8266::closeHTTP(void)
{
    this->sendCommand(Commands::closeHTTP);
    return this->receiveOk(200);
}

bool ESP8266::setFingerPrintHTTP(const std::uint8_t fingerprint[])
{
    this->sendCommand(Commands::setFingerPrintHTTP); 
    for(std::size_t i=0;i<20;i++)
        this->uart->putc(fingerprint[i]);
        
    return this->receiveOk(200);
}

bool ESP8266::setInSecureHTTP()
{
    this->sendCommand(Commands::setInSecureHTTP); 

    return this->receiveOk(200);
}

bool ESP8266::addHeaderHTTP(const std::string &name, const std::string &value)
{
    this->sendCommand(Commands::addHeaderHTTP); 
    this->sendString(name);
    this->sendString(value);
    
    return this->receiveOk(200);
}

std::size_t ESP8266::getResponseHeaderCountHTTP(void)
{
    this->sendCommand(Commands::addHeaderHTTP);
    
    if(this->getResponse(300)!=Response::Data)
        return 0;
    
    if(this->read16()==4)
    {
        std::uint16_t low=this->read16();
        return((this->read16()<<16)|low);
    }
    
    return 0;
}

bool ESP8266::getResponseHeaderHTTP(std::size_t id, std::string &name, std::string &value)
{
    this->sendCommand(Commands::getResponseHeaderHTTP);
    
    this->sendData(reinterpret_cast<const std::uint8_t*>(&value), sizeof(value));
    name = receiveString(200);
    value = receiveString(200);
    
    return true;
}

std::int32_t ESP8266::sendPostHttp(const std::uint8_t* payload, std::uint16_t size, std::uint32_t timeout)
{
    this->sendCommand(Commands::sendPostHttp);
    
    this->write16(size);
    
    for(std::size_t i=0;i<size;i++)
        this->uart->putc(payload[i]);
 
    if(this->getResponse(timeout)!=Response::Data)
        return -1;
    if(this->read16()==4)
    {
        std::uint16_t low=this->read16();
        return ((this->read16()<<16)|low);
    }
    return -1;
}

//
// ESP_NOW
//


bool ESP8266::espNowInit(void)
{
    this->sendCommand(Commands::espNowInit); 

    return this->receiveOk(200);
}

bool ESP8266::espNowAddPeer(const std::string &mac, std::uint8_t channel)
{
    this->sendCommand(Commands::espNowAddPeer);
    this->uart->putc(channel);
    this->sendString(mac);

    return this->receiveOk(200);
}

bool ESP8266::espNowRemovePeer(const std::string &mac)
{
    this->sendCommand(Commands::espNowRemovePeer);
    this->sendString(mac);

    return this->receiveOk(200);
}

bool ESP8266::espNowSend(const std::string &mac, std::uint8_t* buffer, const std::size_t size)
{
    this->sendCommand(Commands::espNowSend);
    this->sendString(mac);
    this->write16(size);
    
    for(std::size_t i=0; i<size;i++)
        this->uart->putc(buffer[i]);
    return this->receiveOk(1000);
}

bool ESP8266::espNowReceive(EspNowReceiveInfo &info)
{
    this->sendCommand(Commands::espNowReceive);
    
    if(this->getResponse(300)!=Response::Data)
        return false;

    return this->readBuffer(reinterpret_cast<uint8_t*>(&info), sizeof(EspNowReceiveInfo), 200);
}

bool ESP8266::espNowDeInit()
{
    this->sendCommand(Commands::espNowDeInit); 

    return this->receiveOk(200);
}


Response ESP8266::getResponse(const std::uint32_t timeout)
{
    const std::uint32_t start = Pokitto::Core::getTime();
	while(true)
	{
		const std::uint32_t now = Pokitto::Core::getTime();
		const std::uint32_t elapsed = (now - start);

		if(elapsed >= timeout)
			break;

		if(this->uart->readable() > 0)
			return static_cast<Response>(this->read16());
	}

	return Response::Error;
}


bool ESP8266::receiveOk(const uint32_t timeout)
{
    return this->getResponse(timeout)==Response::Ok;
}

std::string ESP8266::receiveString(const std::uint32_t timeout)
{
	if(this->getResponse(timeout) != Response::String)
		return "";

	const std::size_t size = this->read16();

	if(size == 0)
		return "";

	std::string result;
	result.reserve(size);

	const std::uint32_t start = Pokitto::Core::getTime();

	while(true)
	{
		const std::uint32_t now = Pokitto::Core::getTime();
		const std::uint32_t elapsed = (now - start);

		if(elapsed >= timeout)
			break;

		while(this->uart->readable() > 0)
		{
			const char c = static_cast<char>(this->uart->getc());

			if(c == '\0')
				continue;

			result += c;

			if(result.size() == size)
            {
                result.shrink_to_fit();
				return result;
            }
		}
	}

	result.shrink_to_fit();

	return result;
}

void ESP8266::sendData(const std::uint8_t* data, const std::uint16_t size)
{
    for(std::size_t i = 0; i < size; ++i)
		this->uart->putc(data[i]);
}

std::uint16_t ESP8266::read16(void)
{
    const std::uint16_t low = static_cast<std::uint8_t>(this->uart->getc());
	const std::uint16_t high = static_cast<std::uint8_t>(this->uart->getc());

	return ((high << 8) | low) ;
}

void ESP8266::write16(const std::uint16_t value)
{
    const std::uint8_t low = (value & 0xFF);
	const std::uint8_t high = ((value >> 8) & 0xFF);

	this->uart->putc(low);
	this->uart->putc(high);
}


void ESP8266::sendCommand(const Commands command)
{
    //flush  uart
    while(this->uart->readable())
        this->uart->getc();
    this->write16(static_cast<std::uint16_t>(command));
}

void ESP8266::sendString(const std::string &String)
{
    this->uart->printf(String.c_str());
    this->uart->putc('\n');
}

std::size_t ESP8266::readBuffer(uint8_t* buffer, const std::size_t bufferSize, const std::uint32_t timeout)
{
    const std::size_t size = this->read16();
    const std::size_t limit = std::min(size, bufferSize);
    const std::uint32_t start = Pokitto::Core::getTime();

    std::size_t index = 0;

    while(index < limit)
    {
        const std::uint32_t now = Pokitto::Core::getTime();
        const std::uint32_t elapsed = (now - start);

        if(elapsed >= timeout)
            break;

        buffer[index] = this->uart->getc();
        index++;
    }

    return index;
}