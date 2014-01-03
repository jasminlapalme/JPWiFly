#include <Arduino.h>

class KeyValue {
private:
	class node {
	public:
		String key;
		String value;
		node* next;
		
		node();
		node(const String& _key, const String& _value);
		~node();
	};
	
	node* nodeForKey(const String& key, node** prev_node = NULL);
	node* nodeForKey_P(const prog_char* key, size_t var_name_len, node** prev_node = NULL);

	node* root;
public:
	KeyValue();
	virtual ~KeyValue();
	
	String encodedFormData();
	void parseEncodedFormData(const String& form_data);
	void setValueForKey(const String& key, const String& value);
	void setValueForKey_P(const prog_char* key, const String& value);
	const String* valueForKey(const String& key);
	const String* valueForKey_P(const prog_char* key);
	const String* valueForKey_P(const prog_char* key, size_t var_name_len);
	void removeKey(const String& key);
	
	void trace();
};
