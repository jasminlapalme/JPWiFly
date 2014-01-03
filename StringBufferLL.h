#include <Arduino.h>
#include <avr/pgmspace.h>

class Stream;

class StringBufferLL {
protected:
	class Node {
	public:
		char* str;
		int length;
		Node* next;
		
		Node(const char* str, int length);
		~Node();
	};

	enum match_state {
		no_match			= -1,
		got_match			= -2,
		partial_match	= -3,
		content_full	= -4
	};

	Stream* serial;
	Node* root;
	Node* last;
	int position;

	void trace();
	bool readAvailable();
	void popRoot();
	match_state match(bool is_prog_char, const char* str);
	int8_t readToFirst(bool is_prog_char, const char* const str[], uint8_t count, String* content = NULL, int16_t timeout = -1);

public:
	StringBufferLL();
	~StringBufferLL();
	
	void setSerial(Stream* s);
	void append(const char* str, int length);
	
	bool readTo(const char* str, String* content = NULL, int16_t timeout = -1);
	bool readTo_P(const prog_char* str, String* content = NULL, int16_t timeout = -1);
	int8_t readToFirst(const char* const str[], uint8_t count, String* content = NULL, int16_t timeout = -1);
	int8_t readToFirst_P(const prog_char* const str[], uint8_t count, String* content = NULL, int16_t timeout = -1);
	bool readNCharacters(size_t count, String* content = NULL);
	void readToTheEnd(String* content = NULL);
};
