#define DEBUG 1

#ifdef DEBUG
#define DPRINT(item) Serial.print(item)
#define DPRINTLN(item) Serial.println(item)

static void DNPRINT_P(const prog_char* str, size_t len) {
	char c;
	if (len == 0)
		return;
	for (size_t i = 0; i < len; i++)
		Serial.print((char)pgm_read_byte(str++));
}

static void DPRINT_P(const prog_char* str) {
	char c;
	if (!str)
		return;
	while ((c = pgm_read_byte(str++)))
		Serial.print(c);
}

#else

#define DPRINT(item)
#define DPRINTLN(item)
#define DPRINT_P(item)
#define DNPRINT_P(item, len)

#endif
