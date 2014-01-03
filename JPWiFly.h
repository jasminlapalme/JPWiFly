/* DHCP modes */
#define WIFLY_DHCP_MODE_OFF			0x00	/* No DHCP, static IP mode */
#define WIFLY_DHCP_MODE_ON			0x01	/* get IP, Gateway, and DNS from AP */
#define WIFLY_DHCP_MODE_AUTOIP	0x02	/* Used with Adhoc networks */
#define WIFLY_DHCP_MODE_CACHE		0x03	/* Use previous DHCP address based on lease */
#define WIFLY_DHCP_MODE_SERVER	0x04	/* Server DHCP IP addresses? */

/* WLAN Join modes */
#define WIFLY_WLAN_JOIN_MANUAL	0x00	/* Don't auto-join a network */
#define WIFLY_WLAN_JOIN_AUTO		0x01	/* Auto-join network set in SSID, passkey, and channel. */
#define WIFLY_WLAN_JOIN_ANY			0x02	/* Ignore SSID and join strongest network using passkey. */
#define WIFLY_WLAN_JOIN_ADHOC		0x04	/* Create an Adhoc network using SSID, Channel, IP and NetMask */
#define WIFLY_WLAN_JOIN_APMODE	0x07	/* Create an Adhoc network using SSID, Channel, IP and NetMask */

#define WIFLY_MODE_WPA			0	
#define WIFLY_MODE_WEP			1

#include <Arduino.h>
#include <Time.h>
#include <avr/pgmspace.h>
#include "StringBufferLL.h"
#include "KeyValue.h"

class Stream;

class JPWiFly {
protected:
	Stream* serial;
	StringBufferLL buffer;
	String prompt;
	bool dhcp;
	bool gotPrompt;
	int exitCommand;
	char replaceChar;

	bool close();
	bool disableDHCP();
	bool enableDHCP();
	bool enterCommandMode();
	bool exitCommandMode();
	bool finishCommand();
	void flushRx(int timeout);
	char getSpaceReplace();
	bool getopt(int opt, String& buf);
  uint32_t getoptInt(int opt, uint8_t base=DEC);
	bool getPrompt();
	bool getSSID(String& buf);
	bool hide();
	bool join(const char *ssid, uint16_t timeout);
	bool join(uint16_t timeout);
	bool match(const prog_char *str, int16_t timeout);
	bool reboot();
	bool save();
	bool setChannel(uint8_t channel);
	bool setDHCP(const uint8_t mode);
	bool setGateway(const prog_char *buf);
	bool setIP(const prog_char *buf);
	bool setJoin(uint8_t join);
	bool setKey(const char *buf);
	bool setNetmask(const prog_char *buf);
	bool setopt(const prog_char *cmd, const char *buf, const prog_char *buf_P = NULL, bool spacesub = false);
	bool setopt(const prog_char *opt, const uint32_t value, uint8_t base = DEC);
	bool setPassphrase(const char *buf);
	bool setPort(const uint16_t port);
	bool setPrompt();
	bool setSpaceReplace(char ch);
	bool setSSID(const char *buf);
	bool setTimeAddress(const char *buf);
	bool setTimeEnable(const uint16_t period);
	bool setTimePort(const uint16_t port);
	bool setTimeZone(const uint8_t zone);
	bool startCommand();
	void writeChar(char c);
	void writeChunk(const char *str);
	void writeChunkln();
	void writeChunkN_P(const prog_char *str, size_t len);
	void writeChunk_P(const prog_char *str);
	void writeData(const char *str);
	void writeDataLn();
	void writeDataLn_P(const prog_char *str);
	void writeData_P(const prog_char *str);
			
public:
	JPWiFly();
	
	void begin(Stream *serial);
	
	bool createApModeNetwork(const char *ssid, int channel = 1, const int port = 80);
	bool joinNetwork(const char* ssid, const char* password, bool dhcp = true, uint8_t mode = WIFLY_MODE_WPA);
	bool getConnection(String& method, String& url, KeyValue& data);
	void sendPage(const prog_char* page, bool(*value)(const prog_char*, size_t, String&, bool*, void*) = NULL, void* context = NULL);
	void setNTP(const char* adr = NULL, uint16_t port = 123, uint8_t zone = 0, uint16_t period = 1);
	bool setDeviceID(const char* device_id);
	time_t getNtpTime();
	bool setCommIdle(int second);
	
	void terminal();
	
	friend class StCommand;
};
