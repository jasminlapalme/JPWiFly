#include "JPWiFly.h"
#include "Stream.h"
#include "Debug.h"

class StCommand {
public:
	JPWiFly* wifly;
	
	StCommand(JPWiFly* w): wifly(w) {
		wifly->startCommand();
	}
	
	~StCommand() {
		wifly->finishCommand();
	}
};

/* Request and response strings in PROGMEM */
const prog_char req_GetIP[] PROGMEM = "get ip\r";
const prog_char resp_IP[] PROGMEM = "IP=";
const prog_char resp_NM[] PROGMEM = "NM=";
const prog_char resp_GW[] PROGMEM = "GW=";
const prog_char resp_Host[] PROGMEM = "HOST=";
const prog_char resp_DHCP[] PROGMEM = "DHCP=";
const prog_char req_GetMAC[] PROGMEM = "get mac\r";
const prog_char resp_MAC[] PROGMEM = "Mac Addr=";
const prog_char req_GetWLAN[] PROGMEM = "get wlan\r";
const prog_char resp_SSID[] PROGMEM = "SSID=";
const prog_char resp_Chan[] PROGMEM = "Chan=";
const prog_char req_GetOpt[] PROGMEM = "get opt\r";
const prog_char resp_DeviceID[] PROGMEM = "DeviceId=";
const prog_char req_GetUart[] PROGMEM = "get u\r";
const prog_char resp_Baud[] PROGMEM = "Baudrate=";
const prog_char req_GetTime[] PROGMEM = "get time\r";
const prog_char resp_Zone[] PROGMEM = "Zone=";
const prog_char req_ShowTime[] PROGMEM = "show time\r";
const prog_char resp_Uptime[] PROGMEM = "UpTime=";
const prog_char resp_Time[] PROGMEM = "Time=";
const prog_char req_GetDNS[] PROGMEM = "get dns\r";
const prog_char resp_DNSAddr[] PROGMEM = "Address=";
const prog_char req_ShowTimeT[] PROGMEM = "show t t\r";
const prog_char resp_RTC[] PROGMEM = "RTC=";
const prog_char resp_Mode[] PROGMEM = "Mode=";
const prog_char req_GetComm[] PROGMEM = "get comm\r";
const prog_char resp_FlushTimeout[] PROGMEM = "FlushTimer=";
const prog_char resp_FlushChar[] PROGMEM = "MatchChar=";
const prog_char resp_FlushSize[] PROGMEM = "FlushSize=";
const prog_char req_GetRSSI[] PROGMEM = "show rssi\r";
const prog_char resp_RSSI[] PROGMEM = "RSSI=(-";
const prog_char resp_Flags[] PROGMEM = "FLAGS=0x";
const prog_char resp_Protocol[] PROGMEM = "PROTO=";
const prog_char req_GetAdhoc[] PROGMEM = "get adhoc\r";
const prog_char resp_Beacon[] PROGMEM = "Beacon=";
const prog_char resp_Probe[] PROGMEM = "Probe=";
const prog_char resp_Reboot[] PROGMEM = "Reboot=";
const prog_char resp_Join[] PROGMEM = "Join=";
const prog_char resp_Rate[] PROGMEM = "Rate=";
const prog_char resp_Power[] PROGMEM = "TxPower=";
const prog_char resp_Replace[] PROGMEM = "Replace=";

/* Request and response for specific info */
static struct {
	const prog_char *req;
	const prog_char *resp;
} requests[] = {
	{ req_GetIP,	resp_IP },	 /* 0 */
	{ req_GetIP,	resp_NM },	 /* 1 */
	{ req_GetIP,	resp_GW },	 /* 2 */
	{ req_GetMAC,	resp_MAC },	 /* 3 */
	{ req_GetWLAN,	resp_SSID },	 /* 4 */
	{ req_GetOpt,	resp_DeviceID }, /* 5 */
	{ req_GetUart,	resp_Baud }, 	 /* 6 */
	{ req_ShowTime,	resp_Time }, 	 /* 7 */
	{ req_ShowTime,	resp_Uptime }, 	 /* 8 */
	{ req_GetTime,	resp_Zone }, 	 /* 9 */
	{ req_GetDNS,	resp_DNSAddr },	 /* 10 */
	{ req_ShowTimeT,	resp_RTC },	 /* 11 */
	{ req_GetIP,	resp_DHCP },	 /* 12 */
	{ req_GetUart,	resp_Mode },	 /* 13 */
	{ req_GetComm,	resp_FlushTimeout }, /* 14 */
	{ req_GetComm,	resp_FlushChar }, /* 15 */
	{ req_GetComm,	resp_FlushSize }, /* 16 */
	{ req_GetRSSI,	resp_RSSI },	 /* 17 */
	{ req_GetIP,	resp_Flags },	 /* 18 */
	{ req_GetIP,	resp_Host },	 /* 19 */
	{ req_GetIP,	resp_Protocol }, /* 20 */
	{ req_GetAdhoc,	resp_Beacon },   /* 21 */
	{ req_GetAdhoc,	resp_Probe },	/* 22 */
	{ req_GetAdhoc,	resp_Reboot },   /* 23 */
	{ req_GetWLAN,	resp_Join },	 /* 24 */
	{ req_GetWLAN,	resp_Rate },	 /* 25 */
	{ req_GetWLAN,	resp_Power },	 /* 26 */
	{ req_GetOpt,	resp_Replace },	 /* 27 */
};

/* Request indices, must match table above */
typedef enum {
	WIFLY_GET_IP	= 0,
	WIFLY_GET_NETMASK	= 1,
	WIFLY_GET_GATEWAY	= 2,
	WIFLY_GET_MAC	= 3,
	WIFLY_GET_SSID	= 4,
	WIFLY_GET_DEVICEID	= 5,
	WIFLY_GET_BAUD	= 6,
	WIFLY_GET_TIME	= 7,
	WIFLY_GET_UPTIME	= 8,
	WIFLY_GET_ZONE	= 9,
	WIFLY_GET_DNS	= 10,
	WIFLY_GET_RTC	= 11,
	WIFLY_GET_DHCP	= 12,
	WIFLY_GET_UART_MODE	= 13,
	WIFLY_GET_FLUSHTIMEOUT = 14,
	WIFLY_GET_FLUSHCHAR	= 15,
	WIFLY_GET_FLUSHSIZE	= 16,
	WIFLY_GET_RSSI	= 17,
	WIFLY_GET_IP_FLAGS	= 18,
	WIFLY_GET_HOST	= 19,
	WIFLY_GET_PROTOCOL	= 20,
	WIFLY_GET_BEACON	= 21,
	WIFLY_GET_PROBE	= 22,
	WIFLY_GET_REBOOT	= 23,
	WIFLY_GET_JOIN	= 24,
	WIFLY_GET_RATE	= 25,
	WIFLY_GET_POWER	= 26,
	WIFLY_GET_REPLACE	= 27,
} e_wifly_requests;

/**
 * Convert a unsigned int to a string
 * @param val the value to convert to a string
 * @param base format for string; either DEC for decimal or
 *             HEX for hexidecimal
 * @param buf the buffer to write the string to
 * @param size the size of the buffer
 * @returns number of characters written to the buffer
 *          not including the null terminator (i.e. size of the string)
 **/
static int simple_utoa(uint32_t val, uint8_t base, char *buf, int size) {
	char tmpbuf[16];
	int ind=0;
	uint32_t nval;
	int fsize=0;

	if (base == DEC) {
		do {
			nval = val / 10;
			tmpbuf[ind++] = '0' + val - (nval * 10);
			val = nval;
		} while (val);
	} else {
		do {
			nval = val & 0x0F;
			tmpbuf[ind++] = nval + ((nval < 10) ? '0' : 'A');
			val >>= 4;
		} while (val);
		tmpbuf[ind++] = 'x';
		tmpbuf[ind++] = '0';
	}

	ind--;

	do {
		buf[fsize++] = tmpbuf[ind];
	} while ((ind-- > 0) && (fsize < (size-1)));
	buf[fsize] = '\0';

	return fsize;
}

/** Simple hex string to uint32_t */
static uint32_t atoh(const char *buf) {
	uint32_t res=0;
	char ch;
	bool gotX = false;

	while ((ch=*buf++) != 0) {
		if (ch >= '0' && ch <= '9')
			res = (res << 4) + ch - '0';
		else if (ch >= 'a' && ch <= 'f')
			res = (res << 4) + ch - 'a' + 10;
		else if (ch >= 'A' && ch <= 'F')
			res = (res << 4) + ch - 'A' + 10;
		else if ((ch == 'x') && !gotX)
			/* Ignore 0x at start */
			gotX = true;
		else
			break;
	}

	return res;
}

/** Simple ASCII to unsigned int */
static uint32_t atou(const char *buf) {
	uint32_t res = 0;

	while (*buf) {
		if ((*buf < '0') || (*buf > '9'))
			break;
		res = res * 10 + *buf - '0';
		buf++;
	}

	return res;
}

JPWiFly::JPWiFly():
	serial(0), buffer(), gotPrompt(false), exitCommand(0)
{}

void JPWiFly::begin(Stream* s) {
	serial = s;
	buffer.setSerial(s);
}

void JPWiFly::writeData(const char *str) {
	DPRINT(F("writeData: "));
	DPRINTLN(str);

	serial->print(str);
	serial->flush();
}

void JPWiFly::writeChar(char c) {
	DPRINT(F("writeChar: "));
	DPRINTLN(c);

	serial->print(c);
}

void JPWiFly::writeData_P(const prog_char *str) {
	DPRINT(F("writeData_P: "));
	DPRINTLN((const __FlashStringHelper *)str);

	serial->print((const __FlashStringHelper *)str);
	serial->flush();
}

void JPWiFly::writeDataLn_P(const prog_char *str) {
	DPRINT(F("writeDataLn_P: "));
	DPRINTLN((const __FlashStringHelper *)str);

	serial->println((const __FlashStringHelper *)str);
	serial->flush();
}

void JPWiFly::writeDataLn() {
	DPRINTLN(F("writeDataLn: "));

	serial->println();
	serial->flush();
}

void JPWiFly::writeChunk(const char *str) {
	serial->println(strlen(str),HEX);
	serial->println(str);
}

void JPWiFly::writeChunkN_P(const prog_char *str, size_t len) {
	serial->println(len,HEX);
	for (size_t i = 0; i < len ; i++)
		serial->write(pgm_read_byte(str++));
	serial->println();
}

void JPWiFly::writeChunk_P(const prog_char *str) {
	serial->println(strlen_P(str),HEX);
	serial->println((const __FlashStringHelper *)str);
}

void JPWiFly::writeChunkln() {
	serial->println('0');
	serial->println();
}

bool JPWiFly::match(const prog_char *str, int16_t timeout) {
	return buffer.readTo_P(str, NULL, timeout);
}

void JPWiFly::flushRx(int timeout) {
	buffer.readToTheEnd();
}

bool JPWiFly::setopt(const prog_char *cmd, const char *buf, const prog_char *buf_P, bool spacesub) {
	StCommand st(this);
	writeData_P(cmd);
	writeData_P(PSTR(" "));
	if (buf) {
		if (spacesub) {
			const char *str = buf;
			char ch;
			/* spaces must be replaced */
			while ((ch = *str++) != 0) {
				if (ch == ' ') {
					writeChar(replaceChar);
				} else {
					writeChar(ch);
				}
			}
		}
		else
			writeData(buf);
	} else if (buf_P) {
		if (spacesub) {
			const prog_char *str = (const prog_char *)buf_P;
			char ch;
			while ((ch = pgm_read_byte(str++)) != 0) {
				if (ch == ' ') {
					writeChar(replaceChar);
				} else {
					writeChar(ch);
				}
			}
		}
		else
			writeData_P(buf_P);
	}
	writeData_P(PSTR("\r"));
	return match(PSTR("AOK\r\n"), 500);
}

bool JPWiFly::setopt(const prog_char *opt, const uint32_t value, uint8_t base) {
	char buf[11];
	simple_utoa(value, base, buf, sizeof(buf));
	return setopt(opt, buf);
}

uint32_t JPWiFly::getoptInt(int opt, uint8_t base) {
	String buf;

	if (!getopt(opt, buf))
		return 0;

	if (base == DEC)
		return atou(buf.c_str());
	else
		return atoh(buf.c_str());
}

/* Get the value of an option */
bool JPWiFly::getopt(int opt, String& buf) {
	StCommand st(this);

	writeData_P(requests[opt].req);
	if (match(requests[opt].resp, 500)) {
		buffer.readTo_P(PSTR("\r\n"), &buf);
		getPrompt();
		return true;
	}
	return false;
}

bool JPWiFly::setPrompt() {
	if (!match(PSTR("<"), 500))
		return false;
	String prompt_content;
	if (!buffer.readTo_P(PSTR(">"), &prompt_content))
		return false;
	
	prompt = "<" + prompt_content + ">";
	gotPrompt = true;
	return true;
}

bool JPWiFly::getPrompt() {
	bool res;
	if (!gotPrompt) {
		DPRINT(F("setPrompt\n\r"));
		res = setPrompt();
		if (!res)
			DPRINT(F("setPrompt failed"));
	} else {
		DPRINT(F("getPrompt \"")); DPRINT(prompt); DPRINT("\"\n\r");
		res = buffer.readTo(prompt.c_str());
	}
	return res;
}

bool JPWiFly::setSpaceReplace(char ch) {
	char buf[2] = { ch, 0 };
	replaceChar = ch;
	return setopt(PSTR("set opt replace"), buf);
}

char JPWiFly::getSpaceReplace() {
	return (char)getoptInt(WIFLY_GET_REPLACE, HEX);
}

bool JPWiFly::setJoin(uint8_t join) {
	return setopt(PSTR("set wlan join"), join);
}

bool JPWiFly::setChannel(uint8_t channel) {
	if (channel > 13)
		channel = 13;
	return setopt(PSTR("set wlan chan"), channel);
}

bool JPWiFly::setSSID(const char *buf) {
	return setopt(PSTR("set wlan ssid"), buf);
}

bool JPWiFly::getSSID(String& buf) {
	return getopt(WIFLY_GET_SSID, buf);
}

bool JPWiFly::setCommIdle(int second) {
	return setopt(PSTR("set comm idle"), second);
}

bool JPWiFly::enterCommandMode() {
	delay(250);
	writeData_P(PSTR("$$$"));
	delay(250);
	
	match(PSTR("CMD\r\n"), 500);

	for (int retry=0; retry < 5; retry++) {
		writeData_P(PSTR("\r"));
		if (match(PSTR("<"), 500)) {
			if (match(PSTR(">"), 500))
				return true;
		}
	}
	delay(250);
	return false;
}

bool JPWiFly::startCommand() {
	if (exitCommand == 0 && !enterCommandMode())
		return false;
	exitCommand++;
	DPRINT("startCommand : ");
	DPRINTLN(exitCommand);
	return true;
}

bool JPWiFly::exitCommandMode() {
	writeData_P(PSTR("exit\r"));

	if (match(PSTR("EXIT\r\n"), 500))
		return true;
	return false;
}

bool JPWiFly::finishCommand() {
	DPRINT("finishCommand : ");
	DPRINTLN(exitCommand);
	if (exitCommand > 0 && --exitCommand == 0)
		return exitCommandMode();
	return true;
}

bool JPWiFly::setDHCP(const uint8_t mode) {
	char buf[2];
	if (mode > 9)
		return false;
	buf[0] = '0' + mode;
	buf[1] = 0;
	return setopt(PSTR("set ip dhcp"), buf);
}

bool JPWiFly::setPort(const uint16_t port) {
	return setopt(PSTR("set ip localport"), port);
}

bool JPWiFly::setIP(const prog_char *buf) {
	return setopt(PSTR("set ip address"), NULL, buf);
}

bool JPWiFly::setNetmask(const prog_char *buf) {
	return setopt(PSTR("set ip netmask"), NULL, buf);
}

bool JPWiFly::setGateway(const prog_char *buf) {
	return setopt(PSTR("set ip gateway"), NULL, buf);
}

bool JPWiFly::setTimeAddress(const char *buf) {
	return setopt(PSTR("set time address"), buf ? buf : "64.90.182.55");
}

bool JPWiFly::setTimePort(const uint16_t port) {
	return setopt(PSTR("set time port"), port);
}

bool JPWiFly::setTimeZone(const uint8_t zone) {
	return setopt(PSTR("set time zone"), zone);
}

bool JPWiFly::setTimeEnable(const uint16_t period) {
	return setopt(PSTR("set time enable"), period);
}

bool JPWiFly::hide() {
	return setopt(PSTR("set wlan hide 1"), (char *)NULL);
}

bool JPWiFly::setKey(const char *buf) {
	if ((buf[1] == 'x') || (buf[1] == 'X')) {
		buf += 2;
	}
	return setopt(PSTR("set wlan key"), buf);
}

bool JPWiFly::setPassphrase(const char *buf) {
	bool res = setopt(PSTR("set wlan phrase"), buf, NULL, true);
	return res;
}

bool JPWiFly::disableDHCP() {
	return setDHCP(WIFLY_DHCP_MODE_OFF);
}

bool JPWiFly::enableDHCP() {
	return setDHCP(WIFLY_DHCP_MODE_ON);
}

bool JPWiFly::save() {
	StCommand st(this);
	writeData_P(PSTR("save\r"));
	return match(PSTR("Storing"), 500);
}

bool JPWiFly::reboot() {
	StCommand st(this);
	
	writeData_P(PSTR("reboot\r"));
	if (!match(PSTR("*Reboot*"), 500))
		return false;
	delay(250);
	exitCommand = 0;
	return true;
}

bool JPWiFly::close()
{
	flushRx(500);
	StCommand st(this);
	writeData_P(PSTR("close\r"));
	return match(PSTR("*CLOS*"), 500);
}

bool JPWiFly::createApModeNetwork(const char *ssid, int channel, const int port) {
	StCommand st(this);
	setJoin(WIFLY_WLAN_JOIN_APMODE);
	setChannel(channel);
	setSSID(ssid);
	setDHCP(WIFLY_DHCP_MODE_SERVER);
	setPort(port);
	setIP(PSTR("1.2.3.4"));
	setNetmask(PSTR("255.255.255.0"));
	setGateway(PSTR("1.2.3.4"));
	
	save();
	reboot();
	return true;
}

bool JPWiFly::join(const char *ssid, uint16_t timeout) {
	const prog_char *joinResult[] = {
		PSTR("FAILED"),
		PSTR("Associated!"),
		PSTR("AUTH-ERR")
	};

	StCommand st(this);

	for (int n_try = 0; n_try < 5; n_try++) {
		for (int n_try_scan = 0; n_try_scan < 5; n_try_scan++) {
			writeData_P(PSTR("scan\r"));
			if (match(PSTR("Found "), 2000)) {
				String n;
				buffer.readNCharacters(1, &n);
				match(PSTR("END:"), 500);
				if (n.length() > 0 && '0' < n[0] && n[0] <= '9')
					break;
			}
			delay(500);
		}
		writeData_P(PSTR("join "));
		if (ssid != NULL)
			writeData(ssid);
		writeData_P(PSTR("\r"));
	
		int8_t res = buffer.readToFirst_P(joinResult, 3, NULL, timeout);
		if (res == 1) {
			// need some time to complete DHCP request
			match(PSTR("GW="), 15000);
			match(PSTR("\r\n"), 0);
			return true;
		}
		delay(1000);
	}
	return false;
}

/** join a wireless network */
bool JPWiFly::join(uint16_t timeout) {
	String ssid;
	getSSID(ssid);
	return join(ssid.c_str(), timeout);
}

bool JPWiFly::joinNetwork(const char *ssid, const char *password, bool dhcp, uint8_t mode) {
	StCommand st(this);

	setJoin(WIFLY_WLAN_JOIN_AUTO);
	setChannel(0);
	setSSID(ssid);
	if (mode == WIFLY_MODE_WPA)
		setPassphrase(password);
	else
		setKey(password);

	if (dhcp)
		enableDHCP();
	return join(ssid, 20000);
}

static const prog_char kGetString[] PROGMEM = "GET ";
static const prog_char kPostString[] PROGMEM = "POST ";
static const prog_char kHeadString[] PROGMEM = "HEAD ";
static const prog_char kNewLineString[] PROGMEM = "\r\n";
static const prog_char kColonString[] PROGMEM = ": ";

static const prog_char * const kProtocolMethodStrings[] =
{
	kGetString,
	kPostString,
	kHeadString
};

static const prog_char * const kColonNewLineStrings[] =
{
	kColonString,
	kNewLineString
};

static String prog_mem_string(const prog_char* prog_str, size_t from, size_t to) {
	String str;
	char c;
	size_t pos = 0;
	size_t str_len = strlen_P(prog_str);
	if (to < 0)
		to = str_len + to;
	while ((c = pgm_read_byte(prog_str++)))
	{
		if (from <= pos)
			str.concat(c);
		pos++;
		if (pos >= to)
			break;
	}
	return str;
}

bool JPWiFly::getConnection(String& method, String& url, KeyValue& data) {
	String line;
	int8_t index = buffer.readToFirst_P(kProtocolMethodStrings, 3);
	if (index == -1)
		return false;
	method = prog_mem_string(kProtocolMethodStrings[index], 0, -1);
	if (!buffer.readTo_P(PSTR(" "), &url))
		return false;
	if (!buffer.readTo_P(kNewLineString))
		return false;
	
	while (true) {
		String key, value;
		int8_t index;
		if ((index = buffer.readToFirst_P(kColonNewLineStrings, 2, &key)) == -1)
			return false;
		if (index == 1)
			break;
		if (!buffer.readTo_P(kNewLineString, &value))
			return false;
		data.setValueForKey(key, value);
	}
	const String* content_length_str = data.valueForKey_P(PSTR("Content-Length"));
	if (!content_length_str)
		return true;
	int content_length = atoi(content_length_str->c_str());
	buffer.readNCharacters(content_length, &line);
	data.setValueForKey("", line);
	return true;
}

static const prog_char *str_Pstr_P(const prog_char *p, const prog_char *q) {
	for( ; pgm_read_byte(p); ++p) {
		const prog_char *p_tmp = p;
		const prog_char *q_tmp = q;
		for ( ; pgm_read_byte(p_tmp) == pgm_read_byte(q_tmp) && pgm_read_byte(q_tmp); ++p_tmp, ++q_tmp);
		if ( !pgm_read_byte(q_tmp) )
			return p;
	}
	return 0;
}

static const prog_char kThreeDots[] PROGMEM = "...";
static const size_t kLenThreeDots = strlen_P(kThreeDots);

void JPWiFly::sendPage(const prog_char* page, bool(*value)(const prog_char*, size_t, String&, bool*, void*), void* context) {
	writeDataLn_P(PSTR("HTTP/1.1 200 OK"));
	writeDataLn_P(PSTR("Content-Type: text/html"));
	writeDataLn_P(PSTR("Transfer-Encoding: chunked"));
	writeDataLn();
	const prog_char* cur_ptr = page;
	const prog_char* next_ptr;
	while ((next_ptr = str_Pstr_P(cur_ptr, kThreeDots)) != NULL) {
		writeChunkN_P(cur_ptr, next_ptr - cur_ptr);
		const prog_char* var_name = next_ptr + kLenThreeDots;
		next_ptr = str_Pstr_P(var_name, kThreeDots);
		if (next_ptr) {
			String content;
			bool isLast = false;
			while (value && !isLast && (*value)(var_name, next_ptr - var_name, content, &isLast, context))
				writeChunk(content.c_str());
			cur_ptr = next_ptr + kLenThreeDots;
		} else
			cur_ptr = var_name;
	}
	writeChunk_P(cur_ptr);
	writeChunkln();
	close();
}

void JPWiFly::setNTP(const char* adr, uint16_t port, uint8_t zone, uint16_t period) {
	StCommand cmd(this);
	setTimeAddress(adr);
	setTimePort(port);
	setTimeZone(zone);
	setTimeEnable(period);
}

bool JPWiFly::setDeviceID(const char* device_id) {
	return setopt(PSTR("set opt deviceid"), device_id);
}

time_t JPWiFly::getNtpTime() {
	return getoptInt(WIFLY_GET_RTC);
}

void JPWiFly::terminal() {
	StCommand st(this);
	DPRINTLN(F("Terminal ready"));
	while (1) {
		if (Serial.available() > 0)
			serial->write(Serial.read());
		if (serial->available())
			Serial.write(serial->read());
	}
}
