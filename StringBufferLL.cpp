#include "StringBufferLL.h"

#include <Arduino.h>
#include "Debug.h"

StringBufferLL::Node::Node(const char* _str, int _length):
	str(new char[_length + 1]), length(_length), next(NULL) {
	strncpy(str, _str, length);
	str[_length] = 0;
}

StringBufferLL::Node::~Node() {
	delete [] str;
}

StringBufferLL::StringBufferLL():
	serial(NULL), root(NULL), last(NULL), position(0)
{}

StringBufferLL::~StringBufferLL() {
	while (root) {
		Node* next_root = root->next;
		delete root;
		root = next_root;
	}
}

void StringBufferLL::setSerial(Stream* s) {
	serial = s;
}

void StringBufferLL::trace() {
	Node* cur_node = root;
	while (cur_node) {
		DPRINT(F("("));
		for (int i = 0; cur_node->str[i] != 0; i++) {
			if (cur_node->str[i] == '\n')
				DPRINT(F("\\n"));
			else if (cur_node->str[i] == '\r')
				DPRINT(F("\\r"));
			else
				DPRINT(cur_node->str[i]);
		}
		if (cur_node == root) {
			DPRINT(F("[R]"));
		}
		if (cur_node == last) {
			DPRINT(F("[L]"));
		}
		DPRINT(F(")"));
		cur_node = cur_node->next;
	}
	DPRINTLN(F(""));
	DPRINTLN(position);
}

// Read the bytes available in serial and append them to the buffer
bool StringBufferLL::readAvailable() {
	char buffer[64];
	int length = 0, total_length = 0;
	do {
		length = serial->readBytes(buffer, sizeof(buffer) - 1);
		buffer[length] = 0;
		total_length += length;
		if (length)
			append(buffer, length);
	} while (length > 0);
	trace();
	return total_length > 0;
}

// Append str of length length to the linked list
void StringBufferLL::append(const char* str, int length) {
	Node* node = new Node(str, length);
	if (last) {
		last->next = node;
		last = node;
	} else {
		root = node;
		last = node;
	}
}

// Delete the root node of the buffer.
void StringBufferLL::popRoot() {
	delete root;
	root = root->next;
	if (root == NULL)
		last = NULL;
}

// Try to match str at the current position in the buffer. If a match is found, the current position
// is push to the end of the match and the previous Nodes are deleted.
StringBufferLL::match_state StringBufferLL::match(bool is_prog_char, const char *str) {
	if (root == NULL)
		return no_match;
	Node* cur_node = root;
	int cur_pos = position;
	int match_pos = 0;
	while (cur_node) {
		char match_car = is_prog_char ? pgm_read_byte(str + match_pos) : str[match_pos];
		
		if (match_car == 0) {
			// End of the string, we have a match. So we delete all the node until the end of the match
			while (root != cur_node)
				popRoot();
			position = cur_pos;
			return got_match;
		}
		
		if (cur_node->str[cur_pos] == match_car)
			match_pos++;
		else
			return no_match;
		
		// the match is still possible, we try at the next position
		cur_pos++;
		if (cur_pos >= cur_node->length) {
			cur_node = cur_node->next;
			cur_pos = 0;
		}
	}
	
	// we are at the end of the buffer, if the matched string is also at the end
	// we have a match
	char match_car = is_prog_char ? pgm_read_byte(str + match_pos) : str[match_pos];
	return match_car ? partial_match : got_match;
}

bool StringBufferLL::readTo(const char *str, String* content, int16_t timeout) {
	return readToFirst(false, &str, 1, content, timeout) == 0;
}

bool StringBufferLL::readTo_P(const prog_char *str, String* content, int16_t timeout) {
	return readToFirst(true, &str, 1, content, timeout) == 0;
}

int8_t StringBufferLL::readToFirst(const char * const str[], uint8_t count, String* content, int16_t timeout) {
	return readToFirst(false, str, count, content, timeout);
}

int8_t StringBufferLL::readToFirst_P(const prog_char * const str[], uint8_t count, String* content, int16_t timeout) {
	return readToFirst(true, str, count, content, timeout);
}

// find the first match string in str and put the current position to the
// end of the match. If no match is found, the buffer comes empty
int8_t StringBufferLL::readToFirst(bool is_prog_char, const char* const str[], uint8_t count, String* content, int16_t timeout) {
	if (content)
		*content = "";
	readAvailable();
	uint32_t start = millis();
	if (root == NULL) {
		while ((root == NULL) && (timeout != -1 && (millis() - start) < timeout))
			readAvailable();
		if (root == NULL)
			return no_match;
	}
	while (true) {
		bool got_partial_match = false;
		for (uint8_t c = 0; c < count; c++) {
			const char* /*PROGMEM*/ str_c = str[c];
			match_state state;
			if ((state = match(is_prog_char, str_c)) == got_match)
				return c;
			if (state == partial_match)
				got_partial_match = true;
		}
		if (got_partial_match) {
			if (readAvailable() || (timeout != -1 && (millis() - start) < timeout))
				continue;
			else
				return no_match;
		}
		if (content)
			*content += root->str[position];
		position++;
		if (position >= root->length) {
			popRoot();
			position = 0;
			if (root == NULL) {
				while ((root == NULL) && (timeout != -1 && (millis() - start) < timeout))
					readAvailable();
				if (root == NULL)
					return no_match;
			}
		}
	}
	return no_match;
}

bool StringBufferLL::readNCharacters(size_t count, String* content) {
	size_t n = 0;
	if (content)
		*content = "";
	readAvailable();
	while (true) {
		if (root == NULL) {
			if (readAvailable())
				continue;
			else
				return false;
		}
		if (content)
			*content += root->str[position];
		position++;
		n++;
		if (position >= root->length) {
			popRoot();
			position = 0;
		}
		if (n >= count)
			return true;
	}
	return false;
}

void StringBufferLL::readToTheEnd(String* content) {
	if (content)
		*content = "";
	readAvailable();
	while (true) {
		if (root == NULL) {
			if (readAvailable())
				continue;
			else
				return;
		}
		if (content)
			*content += (root->str + position);
		popRoot();
		position = 0;
	}
}
