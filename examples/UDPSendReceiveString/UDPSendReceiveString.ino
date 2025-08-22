/*
  UDPSendReceive.pde:
 This sketch receives UDP message strings, prints them to the serial port
 and sends an "acknowledge" string back to the sender

 A Processing sketch is included at the end of file that can be used to send
 and received messages for testing with a computer.

 created 21 Aug 2010
 by Michael Margolis

 This code is in the public domain.
 */

#include <Ethernet3.h>
#include <EthernetUdp2.h>  // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <SPI.h>           // needed for Arduino versions later than 0018

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);

// Init the desired ethernet chip
W5500 chip(10);  // 10 is the CS pin for the W5500 chip
// W5100 chip(10);

EthernetClass Ethernet(&chip);

unsigned int localPort = 8888;  // local port to listen on

// buffers for receiving and sending data
char packetBuffer[24];  // buffer to hold incoming packet, (Using old UDP_MAX_PACKET_SIZE 24, This
                        // is not a limitation of this library or hardware.)
char ReplyBuffer[] = "acknowledged";  // a string to send back

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp(&Ethernet, &chip);

void setup() {
    // start the Ethernet and UDP:
    Ethernet.begin(mac, ip);
    Udp.begin(localPort);

    Serial.begin(9600);
}

void loop() {
    // if there's data available, read a packet
    int packetSize = Udp.parsePacket();
    if (packetSize) {
        Serial.print("Received packet of size ");
        Serial.println(packetSize);
        Serial.print("From ");
        IPAddress remote = Udp.remoteIP();
        for (int i = 0; i < 4; i++) {
            Serial.print(remote[i], DEC);
            if (i < 3) {
                Serial.print(".");
            }
        }
        Serial.print(", port ");
        Serial.println(Udp.remotePort());

        // read the packet into packetBufffer
        Udp.read(packetBuffer, 24);
        Serial.println("Contents:");
        Serial.println(packetBuffer);

        // send a reply, to the IP address and port that sent us the packet we received
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(ReplyBuffer);
        Udp.endPacket();
    }
    delay(10);
}

/*
  Processing sketch to run with this example
 =====================================================

 // Processing UDP example to send and receive string data from Arduino
 // press any key to send the "Hello Arduino" message


 import hypermedia.net.*;

 UDP udp;  // define the UDP object


 void setup() {
 udp = new UDP( this, 6000 );  // create a new datagram connection on port 6000
 //udp.log( true ); 		// <-- printout the connection activity
 udp.listen( true );           // and wait for incoming message
 }

 void draw()
 {
 }

 void keyPressed() {
 String ip       = "192.168.1.177";	// the remote IP address
 int port        = 8888;		// the destination port

 udp.send("Hello World", ip, port );   // the message to send

 }

 void receive( byte[] data ) { 			// <-- default handler
 //void receive( byte[] data, String ip, int port ) {	// <-- extended handler

 for(int i=0; i < data.length; i++)
 print(char(data[i]));
 println();
 }
 */
