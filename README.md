Arduino-IR-Blaster-for-Comcast-DTA-STB-with-Web-Interface
=========================================================

Arduino IR Remote Controll with Web Interface (for comcast DTA STB)

This code uses the following components from sparkfun:
Arduino UNO:			http://www.sparkfun.com/products/11021
Arduino Ethernet Shield:	http://www.sparkfun.com/products/9026
RGB LED:			http://www.sparkfun.com/products/105
IR LED: 			http://www.sparkfun.com/products/10732
IR Reciever: 			http://www.sparkfun.com/products/8554
2 x Pushbutton: 		http://www.sparkfun.com/products/9340
4 x 100 Ohm Resistor		http://www.sparkfun.com/products/10969
2 x 10,000 Ohm Resistor		http://www.sparkfun.com/products/10969
1 x 220 Ohm Resistor		http://www.sparkfun.com/products/10969

This code uses IR code for the comcast DTA remote which is 38Khz XMP IR Protocol.
(AKA: http://www.easypctutorials.com/wp-content/uploads/et_temp/photo2-1104078_958x340.jpg)

To get the on/off times for the IR, I used ladyada raw IR decoder.
Her tutorial can be found here: http://www.ladyada.net/learn/sensors/ir.html
And the IR Decoder is found: https://github.com/adafruit/Raw-IR-decoder-for-Arduino

Instructions on using the IR Decoder, and how to use it.
=========================================================
1) Hook up IR detector/receiver to arduino.
2) Open Serial Monitor on the Arduino, and gently/swiftly press down the key on the remote you wish to capture.
3) Copy/Paste your on/off times (the 2 digit numbers) into respective rows/columns on excel.
4) Repeat step 2-3 at least 2 times.
5) Average the numbers together, and subsequently multiply them by 10 (for micrseconds).
6) Find or create the relevant chan*() function, and Copy/replace the on times into the pulseIR(...) function, and the off times into the delayMicroseconds(...) function. You must maintain the order from your excel spreadsheet.

For clarification purposes,
If the ir dump program outputs the following for the up arrow:
34, 74
32, 158
32, 61

then the code would look like:
void chanUP()
{
  pulseIR(340);
  delayMicroseconds(740);
  pulseIR(320);
  delayMicroseconds(1580);
  pulseIR(320);
  delayMicroseconds(610);
}



