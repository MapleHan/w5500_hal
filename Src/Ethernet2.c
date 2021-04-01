/*
 modified 12 Aug 2013
 by Soohwan Kim (suhwan@wiznet.co.kr)

- 10 Apr. 2015
 Added support for Arduino Ethernet Shield 2
 by Arduino.org team

 */
 
#include "Ethernet2.h"
//#include "Dhcp.h"

// XXX: don't make assumptions about the value of MAX_SOCK_NUM.
IP_AddressTypeDef ETH_dnsServerAddress;
char* ETH_dnsDomainName;
char* ETH_hostName;
//DhcpClass* _dhcp;
//To Do
int ETH_begin_DHCP(uint8_t *mac_address)
{
//   if (_dhcp != NULL) {
//     delete _dhcp;
//   }
//  _dhcp = new DhcpClass();
//  // Initialise the basic info
//  W5500_init(w5500_mosipin,w5500_misopin,w5500_sclkpin,w5500_cspin);
//  W5500_setMACAddress(mac_address);
//  W5500_setIP_AddressTypeDef(IP_AddressTypeDef(0,0,0,0).raw_address());

//  // Now try to get our config info from a DHCP server
//  int ret = _dhcp->beginWithDHCP(mac_address);
//  if(ret == 1)
//  {
//    // We've successfully found a DHCP server and got our configuration info, so set things
//    // accordingly
//    W5500_setIP_AddressTypeDef(_dhcp->getLocalIp().raw_address());
//    W5500_setGatewayIp(_dhcp->getGatewayIp().raw_address());
//    W5500_setSubnetMask(_dhcp->getSubnetMask().raw_address());
//    _dnsServerAddress = _dhcp->getDnsServerIp();
//    _dnsDomainName = _dhcp->getDnsDomainName();
//    _hostName = _dhcp->getHostName();
//  }

//  return ret;
	return 0;
}

void ETH_begin(uint8_t *mac_address, IP_AddressTypeDef* local_ip)
{
  W5500_init();
  W5500_setMACAddress(mac_address);
	IP_AddressTypeDef _addr = *local_ip;
  W5500_setIPAddress(_addr.addr);
	_addr.addr[3] = 1;
  W5500_setGatewayIp(_addr.addr);
	IP_AddressTypeDef subnet = {255, 255, 255, 0};
  W5500_setSubnetMask(subnet.addr);
  ETH_dnsServerAddress = _addr;
}


//int ETH_maintain(){
//  int rc = DHCP_CHECK_NONE;
//  if(_dhcp != NULL){
//    //we have a pointer to dhcp, use it
//    rc = _dhcp->checkLease();
//    switch ( rc ){
//      case DHCP_CHECK_NONE:
//        //nothing done
//        break;
//      case DHCP_CHECK_RENEW_OK:
//      case DHCP_CHECK_REBIND_OK:
//        //we might have got a new IP.
//        W5500_setIP_AddressTypeDef(_dhcp->getLocalIp().raw_address());
//        W5500_setGatewayIp(_dhcp->getGatewayIp().raw_address());
//        W5500_setSubnetMask(_dhcp->getSubnetMask().raw_address());
//        _dnsServerAddress = _dhcp->getDnsServerIp();
//        _dnsDomainName = _dhcp->getDnsDomainName();
//        _hostName = _dhcp->getHostName();
//        break;
//      default:
//        //this is actually a error, it will retry though
//        break;
//    }
//  }
//  return rc;
//}

IP_AddressTypeDef ETH_localIP()
{
  IP_AddressTypeDef ret;
  W5500_getIPAddress(ret.addr);
  return ret;
}

IP_AddressTypeDef ETH_subnetMask()
{
  IP_AddressTypeDef ret;
  W5500_getSubnetMask(ret.addr);
  return ret;
}

IP_AddressTypeDef ETH_gatewayIP()
{
  IP_AddressTypeDef ret;
  W5500_getGatewayIp(ret.addr);
  return ret;
}

IP_AddressTypeDef ETH_dnsServerIP()
{
  return ETH_dnsServerAddress;
}

char* ETH_GetdnsDomainName(){
    return ETH_dnsDomainName;
}

char* ETH_GethostName(){
    return ETH_hostName;
}

