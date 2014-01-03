#include "KeyValue.h"
#include "Debug.h"

static char from_hex(char ch) {
	return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

static char to_hex(int i) {
	return i < 10 ? ('0' + i) : ('A' + (i - 10));
}

static void url_decode(const char *str, int len, String& converted) {
	for (int i = 0; i < len; i++) {
		if (str[i] == '%') {
			if (i + 2 < len && str[i + 1] && str[i + 2]) {
				converted.concat((char)(from_hex(str[i + 1]) << 4 | from_hex(str[i + 2])));
				i += 2;
			}
		}
		else if (str[i] == '+')
			converted.concat(' ');
		else
			converted.concat(str[i]);
	}
}

static String url_encode(const String& str) {
	String encoded;
	for (int i = 0; i < str.length(); i++) {
		if (str[i] >= 0x80)
			encoded += '%' + to_hex((str[i] >> 4) & 0x0F) + to_hex(str[i] & 0x0F);
		else if (str[i] == ' ')
			encoded += '+';
		else
			encoded += str[i];
	}
	return encoded;
}

static bool string_equals_prog_char(const prog_char* prog_str, size_t prog_str_len, const String& str) {
	const char* c_str = str.c_str();
	size_t str_len = str.length();
	while (prog_str_len > 0 && str_len > 0) {
		char c1 = pgm_read_byte(prog_str++);
		char c2 = *c_str++;
		if (c1 != c2)
			return false;
		prog_str_len--;
		str_len--;
	}
	return prog_str_len == 0 && str_len == 0;
}

KeyValue::node::node():
	key(0), value(0), next(NULL)
{}

KeyValue::node::node(const String& _key, const String& _value):
	key(_key), value(_value), next(NULL)
{}

KeyValue::node::~node() {
	if (next)
		delete next;
}

KeyValue::KeyValue():
	root(NULL)
{}

KeyValue::~KeyValue() {
	delete root;
}

KeyValue::node* KeyValue::nodeForKey(const String& key, node** prev_node) {
	if (prev_node)
		*prev_node = NULL;
	node* cur_node = root;
	while (cur_node) {
		if (key.equals(cur_node->key))
			return cur_node;
		if (prev_node)
			*prev_node = cur_node;
		cur_node = cur_node->next;
	}
	return NULL;
}

KeyValue::node* KeyValue::nodeForKey_P(const prog_char* key, size_t key_len, node** prev_node) {
	if (prev_node)
		*prev_node = NULL;
	node* cur_node = root;
	while (cur_node) {
		if (string_equals_prog_char(key, key_len, cur_node->key))
			return cur_node;
		if (prev_node)
			*prev_node = cur_node;
		cur_node = cur_node->next;
	}
	return NULL;
}

void KeyValue::setValueForKey(const String& key, const String& value) {
	node* node = nodeForKey(key);
	if (node)
		node->value = value;
	else {
		KeyValue::node* new_node = new KeyValue::node(key, value);
		new_node->next = root;
		root = new_node;
	}
}

void KeyValue::setValueForKey_P(const prog_char* key, const String& value) {
	node* node = nodeForKey_P(key, strlen_P(key));
	if (node)
		node->value = value;
	else {
		KeyValue::node* new_node = new KeyValue::node(key, value);
		new_node->next = root;
		root = new_node;
	}
}

const String* KeyValue::valueForKey(const String& key) {
	node* node = nodeForKey(key);
	if (node)
		return &node->value;
	return NULL;
}

const String* KeyValue::valueForKey_P(const prog_char* key) {
	node* node = nodeForKey_P(key, strlen_P(key));
	if (node)
		return &node->value;
	return NULL;
}

const String* KeyValue::valueForKey_P(const prog_char* key, size_t var_name_len) {
	node* node = nodeForKey_P(key, var_name_len);
	if (node)
		return &node->value;
	return NULL;
}

void KeyValue::removeKey(const String& key) {
	node* prev_node;
	node* node = nodeForKey(key, &prev_node);
	if (!node)
		return;
	if (node == root) {
		KeyValue::node* new_root = root->next;
		root->next = NULL;
		delete root;
		root = new_root;
	} else {
		prev_node->next = node->next;
		node->next = NULL;
		delete node;
	}
}

void KeyValue::trace() {
	node* cur_node = root;
	while (cur_node) {
		DPRINT(cur_node->key);
		DPRINT_P(PSTR(" : "));
		DPRINTLN(cur_node->value);
		cur_node = cur_node->next;
	}
}

void KeyValue::parseEncodedFormData(const String& form_data) {
	int cur_pos = 0;
	while (cur_pos < form_data.length()) {
		int next_pos = form_data.indexOf('=', cur_pos);
		if (next_pos == -1)
			break;
		String var_name;
		url_decode(form_data.c_str() + cur_pos, next_pos - cur_pos, var_name);
		cur_pos = next_pos + 1;

		next_pos = form_data.indexOf('&', cur_pos);
		if (next_pos == -1)
			next_pos = form_data.length();
		String value;
		url_decode(form_data.c_str() + cur_pos, next_pos - cur_pos, value);

		setValueForKey(var_name, value);
		cur_pos = next_pos + 1;
	}
}

String KeyValue::encodedFormData() {
	String form_data;
	node* cur_node = root;
	while (cur_node) {
		form_data += url_encode(cur_node->key) + "=" + url_encode(cur_node->value);
		if (cur_node->next)
			form_data += "&";
		cur_node = cur_node->next;
	}
	return form_data;
}
