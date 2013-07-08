Arduino-IR-Blaster-for-Comcast-DTA-STB-with-Web-Interface
=========================================================
Arduino IR Remote Controll with Web Interface (for comcast DTA STB)

This code is part of an Arduino Tutorial which can be found here: http://colinbookman.com/arduino_ir_blaster/

This code uses the following components from sparkfun: <ul>
<li>Arduino UNO:  		http://www.sparkfun.com/products/11021 </li>
<li>Arduino Ethernet Shield:	http://www.sparkfun.com/products/9026 </li>
<li>RGB LED:			http://www.sparkfun.com/products/105 </li>
<li>IR LED: 			http://www.sparkfun.com/products/10732 </li>
<li>IR Reciever: 			http://www.sparkfun.com/products/8554 </li>
<li>2 x Pushbutton: 		http://www.sparkfun.com/products/9340 </li>
<li>4 x 100 Ohm Resistor		http://www.sparkfun.com/products/10969</li>
<li>2 x 10,000 Ohm Resistor		http://www.sparkfun.com/products/10969</li>
<li>1 x 220 Ohm Resistor		http://www.sparkfun.com/products/10969</li>
</ul>

This code uses IR code for the comcast DTA remote which is 38Khz XMP IR Protocol.<br />
(AKA: http://www.easypctutorials.com/wp-content/uploads/et_temp/photo2-1104078_958x340.jpg)

Please refer to the <a href="https://github.com/cobookman/Arduino-IR-Blaster-for-Comcast-DTA-STB-with-Web-Interface/blob/master/schematic.jpg">Schematic</a> for pin layout. Not listed is the IR Receiver, which has:<br />
VCC -> +5v <br />
CTL -> Pin 4 <br />
GND -> GND

To get the on/off times for the IR, I used ladyada raw IR decoder. <br />
Her tutorial can be found here: http://www.ladyada.net/learn/sensors/ir.html<br />
And the IR Decoder is found: https://github.com/adafruit/Raw-IR-decoder-for-Arduino<br />

Instructions on using the IR Decoder.
=========================================================
<ol>
<li>Hook up IR detector/receiver to arduino.</li>
<li>Open Serial Monitor on the Arduino, and gently/swiftly press down the key on the remote you wish to capture.</li>
<li>Copy/Paste your on/off times (the 2 digit numbers) into respective rows/columns on excel.</li>
<li>Repeat step 2-3 at least 2 times.</li>
<li>Average the numbers together, and subsequently multiply them by 10 (for micrseconds).</li>
<li>Find or create the relevant chan*() function, and Copy/replace the on times into the pulseIR(...) function, and the off times into the delayMicroseconds(...) function. You must maintain the order from your excel spreadsheet.</li>
</ol>

For clarification purposes,<br />
If the ir dump program outputs the following for the up arrow:<br />
34, 74<br />
32, 158<br />
32, 61<br />

then the code would look like:<br />
void chanUP()<br />
{<br />
  pulseIR(340); <br />
  delayMicroseconds(740);<br />
  pulseIR(320);<br />
  delayMicroseconds(1580);<br />
  pulseIR(320);<br />
  delayMicroseconds(610);<br />
}<br />
