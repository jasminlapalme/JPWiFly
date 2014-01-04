JPWiFly
=======

An Arduino library for the Roving Networks WiFly RN-XV.

This library has been highly inspired by WiFlyHQ (https://github.com/harlequin-tech/WiFlyHQ). It has been updated to work with version 4.0x of the WiFly firmware.

##Features

* connect to a WEP or WAP wireless network
* create an access point (AP mode)
* use NTP to get date and time
* create and HTTP server

##Examples

    #include <JPWiFly.h>
    
    SoftwareSerial mySerial(10, 11);
    JPWiFly wiFly;

    time_t getNtpTime()
    {
    	return wiFly.getNtpTime();
    }

    void setup()
    {
    	mySerial.begin(9600);
    	wiFly.begin(&mySerial);

    	wiFly.setDeviceID("JPWiFly");
    	
    	if (wiFly.joinNetwork("network", "password"))
    		wiFly.setNTP();
    	else
    		wiFly.createApModeNetwork("JPWiFly");
    
    	setTime( wiFly.getNtpTime() );
    	setSyncProvider( getNtpTime );
    }

    void loop()
    {
    	String method;
    	String url;
    	KeyValue data;
    	if (wiFly.getConnection(method, url, data))
    	{
    		// data contains key-value pairs of the http headers
    		if (url == "/")
    		{
    			wiFly.sendPage(F("<html><head><title>JPWiFly</title></head>"
    			                 "<body><h1>JPWiFly</h1></body></html>"));
    		}
    	}
    }
