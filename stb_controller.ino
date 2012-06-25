// this code is public domain

#include <SPI.h>
#include <Ethernet.h>


const int IRledPin =  4;    // IR LED
const int upPin = 2;        // Up Push-button
const int dPin = 3;         // Down Push-button
const int rLED = 7;
const int gLED = 6;
const int bLED = 5;

int upState = 0;
int dState = 0;


volatile int webCmd = -1;

byte mac[] = {
  0x90, 0xA2, 0xDA, 0x00, 0x54, 0x48 }; // Mac Address of ethernet shield
EthernetServer server(80);                // Start server on port 80
//Hex definitions
#define CHANUP 0x80
#define CHANDOWN 0x90
#define CHAN1 0x81
#define CHAN2 0x82
#define CHAN3 0x83
#define CHAN4 0x84
#define CHAN5 0x85
#define CHAN6 0x86
#define CHAN7 0x87
#define CHAN8 0x88
#define CHAN9 0x89
#define CHAN0 0x01
//define LED numerical representation for program
const int red = 1;
const int green = 2;
const int blue = 3;



void setup()   {                
  // initialize the IR digital pin as an output:
  pinMode(IRledPin, OUTPUT);      
  pinMode(upState, INPUT);    
  pinMode(dState, INPUT); 
  pinMode(gLED, OUTPUT);
  pinMode(bLED, OUTPUT);
  pinMode(rLED, OUTPUT);
  //Turn on Red LED as a Hello World
  led(red);
  Serial.begin(9600);
  Serial.println("Inputs declared, moving on to Ethernet"); 
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP"); //Ethernet Failed, allow buttons to still work though
    //Blink LED between Red, green and blue once to exclaim error
    led(red);
    delay(200);
    led(green);
    delay(200);
    led(blue);
    delay(200);
  }
  else {
    //Blink quickly to blue to say internet success!
    led(blue);
    delay(100);
    led(red);

    // print your local IP address:
    Serial.print("My IP address: ");
    for (byte thisByte = 0; thisByte < 4; thisByte++) {
      // print the value of each byte of the IP address:
      Serial.print(Ethernet.localIP()[thisByte], DEC);
      Serial.print("."); 
    }
    //Serial.println();
    // Start webserver
    server.begin();
  }
}//setup


void loop()                     
{
  //====This code is for the Push Buttons which are mounted on the front===
  //====                          of the device                         ====
  upState = digitalRead(upPin);
  dState = digitalRead(dPin);
  if(upState == HIGH)   //Up Pushbutton Pressed, go up a channel
  {
    led(blue);  //go to Blue LED state (notification purposes)
    chanUP(); 
    delay(200);
    led(red);   //go to RED LED state (notification purposes)
  }

  if(dState == HIGH)  //Down Pushbutton pressed, go down a channel
  {
    led(blue);
    chanD(); 
    delay(200);
    led(red);
  }
  //===Start listening for web commands===
  WebServerLoop();
  
  //====Act on webserver commands======
  if(webCmd !=-1)
  {
    switch(webCmd)       //switch between buttons # 0-9 as well as up/down (web interface)
    {                   
      case CHANUP:   led(green); chanUP(); led(red); break;
      case CHANDOWN: led(green); chanD();  led(red); break;
      case CHAN1: led(green); chan1(); led(red); break;
      case CHAN2: led(green); chan2(); led(red); break;
      case CHAN3: led(green); chan3(); led(red); break;
      case CHAN4: led(green); chan4(); led(red); break;
      case CHAN5: led(green); chan5(); led(red); break;
      case CHAN6: led(green); chan6(); led(red); break;
      case CHAN7: led(green); chan7(); led(red); break;
      case CHAN8: led(green); chan8(); led(red); break;
      case CHAN9: led(green); chan9(); led(red); break;
      case CHAN0: led(green); chan0(); led(red); break;
    }

    //reset comd
    webCmd = -1;
  }
}
void led(int color)
{
 switch(color) {
  case 1: analogWrite(rLED, 255); analogWrite(gLED, 0); analogWrite(bLED, 0); break;
  case 2: analogWrite(rLED, 0); analogWrite(gLED, 255); analogWrite(bLED, 0); break;
  case 3: analogWrite(rLED, 0); analogWrite(gLED, 0); analogWrite(bLED, 255); break;
  default: analogWrite(rLED, 255); analogWrite(gLED, 0); analogWrite(bLED, 0); break;
 } 
}
void ParseHttpHeader(Client &client)
{
  char c;

  // Skip through until we hit a question mark (first one)
  while((c = client.read()) != '?' && client.available())
  {
    // Debug - print data
    //Serial.print(c);
  }

  // Are we here for a question mark or did we run out of data?
  if(client.available() > 2)
  {
    char pcUrlNum[3], *pc;

    // We have enough data for a hex number - read it
    for(int i=0; i < 2; i++)
    {
      // Read and dump data to debug port
      //Serial.print(c = pcUrlNum[i] = client.read());
      c = pcUrlNum[i] = client.read();
    }
    // Null terminate string
    pcUrlNum[2] = '\0';

    // Get hex number
    webCmd = strtol(pcUrlNum, &pc, 0x10);  
  }

  // Skip through and discard all remaining data
  while(client.available())
  {
    // Debug - print data
    //Serial.print(c = client.read());
    c = client.read();
  }
}
void SubmitButton(Client &client, char *pcLabel, int iCmd)
{
  client.print("<form method=post action=\"/?");
  client.print(iCmd, HEX);
  client.print("\"><input type=submit value=\"");
  client.print(pcLabel);
  client.print("\" name=\" RSCmd \">");
  client.println("</form>");  
}
void WebServerLoop()
{  
  EthernetClient client = server.available();
  boolean bPendingHttpResponse = false; // True when we've received a whole HTTP request and need to output the webpage
  char c;  // For reading in HTTP request one character at a time

  if (client) {
    // Loop as long as there's a connection
    while (client.connected()) {
      // Do we have pending data (an HTTP request)?     
      if (client.available()) {

        // Indicate we need to respond to the HTTP request as soon as we're done processing it
        bPendingHttpResponse = true;

        ParseHttpHeader(client);        
      }
      else
      {
        // There's no data waiting to be read in on the client socket.  Do we have a pending HTTP request?
        if(bPendingHttpResponse)
        {
          // Yes, we have a pending request.  Clear the flag and then send the webpage to the client
          bPendingHttpResponse = false;
          client.println("<html><body>AEPi TV Remote<table>");
          //Display Channels 1,2,3
            client.println("<tr><td>");
            SubmitButton(client, "1", CHAN1);
            client.println("</td><td>");
            SubmitButton(client, "2", CHAN2);
            client.println("</td><td>");
            SubmitButton(client, "3", CHAN3);
            client.println("</td></tr>");
          //Display Channels 4,5,6
            client.println("<tr><td>");
            SubmitButton(client, "4", CHAN4);
            client.println("</td><td>");
            SubmitButton(client, "5", CHAN5);
            client.println("</td><td>");
            SubmitButton(client, "6", CHAN6);
            client.println("</td></tr>");
          // Display Channels, 7,8,9
            client.println("<tr><td>");
            SubmitButton(client, "7", CHAN7);
            client.println("</td><td>");
            SubmitButton(client, "8", CHAN8);
            client.println("</td><td>");
            SubmitButton(client, "9", CHAN9);
            client.println("</td></tr>");
          // Display Chan UP, 0, Chan Down
            client.println("<tr><td>");
            SubmitButton(client, "&uarr;", CHANUP);
            client.println("</td><td>");
            SubmitButton(client, "0", CHAN0);
            client.println("</td><td>");
            SubmitButton(client, "&darr;", CHANDOWN);
            client.println("</td></tr>");

          //SubmitButton(client, "Up", CHANUP);
          //client.println("</td><tr><td>");
          //SubmitButton(client, "Down", CHANDOWN);
          //client.println("</tr></td>");
          client.println("</table></body></html>");
          delay(1);
          client.stop();
        }
      }
    }//end while        
  } //End first IF
}



// This procedure sends a 38KHz pulse to the IRledPin 
// for a certain # of microseconds. We'll use this whenever we need to send codes
void pulseIR(long microsecs) {
  // we'll count down from the number of microseconds we are told to wait

  cli();  // this turns off any background interrupts

  while (microsecs > 0) {
    // 38 kHz is about 13 microseconds high and 13 microseconds low
    digitalWrite(IRledPin, HIGH);  // this takes about 3 microseconds to happen
    delayMicroseconds(10);         // hang out for 10 microseconds
    digitalWrite(IRledPin, LOW);   // this also takes about 3 microseconds
    delayMicroseconds(10);         // hang out for 10 microseconds

    // so 26 microseconds altogether
    microsecs -= 26;
  }

  sei();  // this turns them back on
}

void chanUP() {
  // This is the code for my particular Nikon, for others use the tutorial
  // to 'grab' the proper code from the remote
  pulseIR(340);
  delayMicroseconds(750);
  pulseIR(320);
  delayMicroseconds(1590);
  pulseIR(300);
  delayMicroseconds(640);
  pulseIR(300);
  delayMicroseconds(2680);
  pulseIR(300);
  delayMicroseconds(1180);
  pulseIR(300);
  delayMicroseconds(1180);
  pulseIR(300);
  delayMicroseconds(1050);
  pulseIR(310);
  delayMicroseconds(2530);
  pulseIR(300);
  delayMicroseconds(12610);
  pulseIR(320);
  delayMicroseconds(780);
  pulseIR(300);
  delayMicroseconds(910);
  pulseIR(310);
  delayMicroseconds(630);
  pulseIR(310);
  delayMicroseconds(640);
  pulseIR(300);
  delayMicroseconds(640);
  pulseIR(300);
  delayMicroseconds(2400);
  pulseIR(300);
  delayMicroseconds(640);
  pulseIR(300);
  delayMicroseconds(640);
  pulseIR(300);
  delayMicroseconds(0);
  delay(65); // wait 65 milliseconds before sending it again
  pulseIR(2000);
  delay(27);
  pulseIR(440);
  delayMicroseconds(1500);
  pulseIR(460);
  delayMicroseconds(3440);
  pulseIR(480);
}

void chanD () 
{
  pulseIR(320);
  delayMicroseconds(765);  
  pulseIR(320);
  delayMicroseconds(1570); 
  pulseIR(330);
  delayMicroseconds(610);
  pulseIR(330);
  delayMicroseconds(2640);
  pulseIR(330);
  delayMicroseconds(1160);
  pulseIR(330);
  delayMicroseconds(1030);
  pulseIR(310);
  delayMicroseconds(2530);
  pulseIR(310);
  delayMicroseconds(12600);
  pulseIR(340);
  delayMicroseconds(760);
  pulseIR(320);
  delayMicroseconds(750);
  pulseIR(320);
  delayMicroseconds(620);
  pulseIR(320);
  delayMicroseconds(620);
  pulseIR(320);
  delayMicroseconds(620);
  pulseIR(320);
  delayMicroseconds(2540);
  pulseIR(290);
  delayMicroseconds(640);
  pulseIR(310);  
  delayMicroseconds(630);
  pulseIR(300);
  delayMicroseconds(0); 
}
void chan1() 
{
  pulseIR(320);
  delayMicroseconds(780); 
  pulseIR(300);
  delayMicroseconds(1580); 
  pulseIR(300);
  delayMicroseconds(640);
  pulseIR(300);
  delayMicroseconds(2680);
  pulseIR(300);
  delayMicroseconds(1180);
  pulseIR(300);
  delayMicroseconds(1180);
  pulseIR(310);
  delayMicroseconds(1050);
  pulseIR(300);
  delayMicroseconds(2540);
  pulseIR(300);
  delayMicroseconds(12600);
  pulseIR(340);
  delayMicroseconds(760);
  pulseIR(310);
  delayMicroseconds(2530);
  pulseIR(300);
  delayMicroseconds(640);
  pulseIR(320);
  delayMicroseconds(620);
  pulseIR(300);
  delayMicroseconds(640);
  pulseIR(320);
  delayMicroseconds(760);
  pulseIR(300);  
  delayMicroseconds(640);
  pulseIR(300);
  delayMicroseconds(640); 
  pulseIR(300);
  delayMicroseconds(0);
}
void chan2() 
{
  pulseIR(340);
  delayMicroseconds(750); 
  pulseIR(320);
  delayMicroseconds(1570); 
  pulseIR(320);
  delayMicroseconds(620);
  pulseIR(320);
  delayMicroseconds(2660);
  pulseIR(320);
  delayMicroseconds(1160);
  pulseIR(320);
  delayMicroseconds(1160);
  pulseIR(320);
  delayMicroseconds(1030);
  pulseIR(320);
  delayMicroseconds(2510);
  pulseIR(320);
  delayMicroseconds(12610);
  pulseIR(330);
  delayMicroseconds(760);
  pulseIR(320);
  delayMicroseconds(2380);
  pulseIR(320);
  delayMicroseconds(620);
  pulseIR(320);
  delayMicroseconds(610);
  pulseIR(330);
  delayMicroseconds(620);
  pulseIR(320);
  delayMicroseconds(890);
  pulseIR(330);  
  delayMicroseconds(620);
  pulseIR(320);
  delayMicroseconds(620); 
  pulseIR(320);
  delayMicroseconds(0);
}
void chan3() 
{
  pulseIR(330);
  delayMicroseconds(770); 
  pulseIR(310);
  delayMicroseconds(1580); 
  pulseIR(300);
  delayMicroseconds(640);
  pulseIR(300);
  delayMicroseconds(2680);
  pulseIR(300);
  delayMicroseconds(1180);
  pulseIR(300);
  delayMicroseconds(1180);
  pulseIR(310);
  delayMicroseconds(1050);
  pulseIR(300);
  delayMicroseconds(2520);
  pulseIR(320);
  delayMicroseconds(12600);
  pulseIR(330);
  delayMicroseconds(770);
  pulseIR(300);
  delayMicroseconds(2260);
  pulseIR(320);
  delayMicroseconds(630);
  pulseIR(310);
  delayMicroseconds(630);
  pulseIR(300);
  delayMicroseconds(640);
  pulseIR(310);
  delayMicroseconds(1030);
  pulseIR(310);  
  delayMicroseconds(630);
  pulseIR(310);
  delayMicroseconds(630); 
  pulseIR(310);
  delayMicroseconds(0);
}
void chan4()
{
  pulseIR(330);
  delayMicroseconds(760); 
  pulseIR(330);
  delayMicroseconds(1570); 
  pulseIR(310);
  delayMicroseconds(640);
  pulseIR(300);
  delayMicroseconds(2660);
  pulseIR(320);
  delayMicroseconds(1160);
  pulseIR(320);
  delayMicroseconds(1180);
  pulseIR(300);
  delayMicroseconds(1030);
  pulseIR(320);
  delayMicroseconds(2530);
  pulseIR(300);
  delayMicroseconds(12620);
  pulseIR(320);
  delayMicroseconds(780);
  pulseIR(300);
  delayMicroseconds(2120);
  pulseIR(320);
  delayMicroseconds(610);
  pulseIR(330);
  delayMicroseconds(610);
  pulseIR(330);
  delayMicroseconds(620);
  pulseIR(320);
  delayMicroseconds(1180);
  pulseIR(300);  
  delayMicroseconds(620);
  pulseIR(320);
  delayMicroseconds(630); 
  pulseIR(310);
  delayMicroseconds(0);
}
void chan5()
{
  pulseIR(330);
  delayMicroseconds(740); 
  pulseIR(330);
  delayMicroseconds(1570); 
  pulseIR(320);
  delayMicroseconds(620);
  pulseIR(320);
  delayMicroseconds(2660);
  pulseIR(320);
  delayMicroseconds(1170);
  pulseIR(320);
  delayMicroseconds(1150);
  pulseIR(320);
  delayMicroseconds(1020);
  pulseIR(330);
  delayMicroseconds(2530);
  pulseIR(310);
  delayMicroseconds(12600);
  pulseIR(340);
  delayMicroseconds(750);
  pulseIR(320);
  delayMicroseconds(1990);
  pulseIR(310);
  delayMicroseconds(630);
  pulseIR(310);
  delayMicroseconds(630);
  pulseIR(310);
  delayMicroseconds(630);
  pulseIR(310);
  delayMicroseconds(1310);
  pulseIR(310);  
  delayMicroseconds(630);
  pulseIR(310);
  delayMicroseconds(620); 
  pulseIR(320);
  delayMicroseconds(0);
}
void chan6()
{
  pulseIR(340);
  delayMicroseconds(740); 
  pulseIR(320);
  delayMicroseconds(1580); 
  pulseIR(320);
  delayMicroseconds(610);
  pulseIR(330);
  delayMicroseconds(2660);
  pulseIR(320);
  delayMicroseconds(1160);
  pulseIR(320);
  delayMicroseconds(1170);
  pulseIR(310);
  delayMicroseconds(1030);
  pulseIR(310);
  delayMicroseconds(2540);
  pulseIR(300);
  delayMicroseconds(12610);
  pulseIR(330);
  delayMicroseconds(760);
  pulseIR(320);
  delayMicroseconds(1850);
  pulseIR(310);
  delayMicroseconds(630);
  pulseIR(310);
  delayMicroseconds(620);
  pulseIR(320);
  delayMicroseconds(610);
  pulseIR(330);
  delayMicroseconds(1440);
  pulseIR(320);  
  delayMicroseconds(610);
  pulseIR(340);
  delayMicroseconds(610); 
  pulseIR(320);
  delayMicroseconds(0);
}
void chan7()
{
  pulseIR(320);
  delayMicroseconds(760); 
  pulseIR(320);
  delayMicroseconds(1580); 
  pulseIR(310);
  delayMicroseconds(630);
  pulseIR(320);
  delayMicroseconds(2640);
  pulseIR(330);
  delayMicroseconds(1160);
  pulseIR(320);
  delayMicroseconds(1160);
  pulseIR(330);
  delayMicroseconds(1020);
  pulseIR(320);
  delayMicroseconds(2520);
  pulseIR(320);
  delayMicroseconds(12600);
  pulseIR(340);
  delayMicroseconds(750);
  pulseIR(320);
  delayMicroseconds(1710);
  pulseIR(320);
  delayMicroseconds(620);
  pulseIR(320);
  delayMicroseconds(620);
  pulseIR(320);
  delayMicroseconds(620);
  pulseIR(320);
  delayMicroseconds(1580);
  pulseIR(310);  
  delayMicroseconds(620);
  pulseIR(320);
  delayMicroseconds(620); 
  pulseIR(320);
  delayMicroseconds(0);
}
void chan8()
{
  pulseIR(340);
  delayMicroseconds(750); 
  pulseIR(330);
  delayMicroseconds(1560); 
  pulseIR(320);
  delayMicroseconds(610);
  pulseIR(330);
  delayMicroseconds(2660);
  pulseIR(320);
  delayMicroseconds(1170);
  pulseIR(310);
  delayMicroseconds(1170);
  pulseIR(310);
  delayMicroseconds(1030);
  pulseIR(320);
  delayMicroseconds(2520);
  pulseIR(330);
  delayMicroseconds(12600);
  pulseIR(320);
  delayMicroseconds(770);
  pulseIR(310);
  delayMicroseconds(1560);
  pulseIR(330);
  delayMicroseconds(620);
  pulseIR(330);
  delayMicroseconds(600);
  pulseIR(340);
  delayMicroseconds(610);
  pulseIR(330);
  delayMicroseconds(1700);
  pulseIR(320);  
  delayMicroseconds(610);
  pulseIR(330);
  delayMicroseconds(610); 
  pulseIR(330);
  delayMicroseconds(0);
}
void chan9()
{
  pulseIR(330);
  delayMicroseconds(760); 
  pulseIR(320);
  delayMicroseconds(1580); 
  pulseIR(300);
  delayMicroseconds(630);
  pulseIR(310);
  delayMicroseconds(2660);
  pulseIR(320);
  delayMicroseconds(1160);
  pulseIR(330);
  delayMicroseconds(1160);
  pulseIR(320);
  delayMicroseconds(1030);
  pulseIR(320);
  delayMicroseconds(2520);
  pulseIR(320);
  delayMicroseconds(12600);
  pulseIR(330);
  delayMicroseconds(760);
  pulseIR(320);
  delayMicroseconds(1430);
  pulseIR(320);
  delayMicroseconds(620);
  pulseIR(320);
  delayMicroseconds(620);
  pulseIR(320);
  delayMicroseconds(630);
  pulseIR(310);
  delayMicroseconds(1850);
  pulseIR(310);  
  delayMicroseconds(630);
  pulseIR(310);
  delayMicroseconds(630); 
  pulseIR(320);
  delayMicroseconds(0);
}
void chan0()
{
  pulseIR(320);
  delayMicroseconds(760); 
  pulseIR(320);
  delayMicroseconds(1560);
  pulseIR(340);
  delayMicroseconds(620);
  pulseIR(330);
  delayMicroseconds(2660);
  pulseIR(310);
  delayMicroseconds(1180);
  pulseIR(330);
  delayMicroseconds(1180);
  pulseIR(330);
  delayMicroseconds(1040);
  pulseIR(310);
  delayMicroseconds(2520);
  pulseIR(320);
  delayMicroseconds(12600);
  pulseIR(340);
  delayMicroseconds(750);
  pulseIR(320);
  delayMicroseconds(2650);
  pulseIR(290);
  delayMicroseconds(620);
  pulseIR(310);
  delayMicroseconds(630);
  pulseIR(310);
  delayMicroseconds(640);
  pulseIR(300);
  delayMicroseconds(640);
  pulseIR(300);  
  delayMicroseconds(630);
  pulseIR(310);
  delayMicroseconds(640); 
  pulseIR(300);
  delayMicroseconds(0);
}
