# habits-box

A small Arduino project for recording whether or not I complete daily habits. 
As a challenge I wired up 12 digital inputs to 3 analog input pins on the arduino, which is why I have all that code for converting analog signals into bit strings.

I used the [Firebase-Arduino-WiFiNINA](https://github.com/mobizt/Firebase-Arduino-WiFiNINA) library for the Firebase integration.

Note: When configuring firebase don't forget to update the SSL certificates for the WifiNina firmware. [See here](https://github.com/mobizt/Firebase-Arduino-WiFi101/issues/9#issuecomment-841571945)