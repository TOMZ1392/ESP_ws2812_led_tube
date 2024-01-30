/*
    ESP01 NEOLIGHT WS2812B CRGB LED BAR

    To control CRGB WS2812B ledstrip from any device with a browser
    ESP01 wifi server takes requests and drive leds accordingly

    The circuit:
    * 5V 1A power supply
    * ESP01
    * RX pin 1 pin of UART used for led data
    * ws2812 led strip with 68 leds

    2024-JAN-20
    By Tomz1392
    Modified 2024-JAN-20
    By Tomz1392

    http://url/of/online/tutorial.cc

*/

// Load Wi-Fi library
#include <ESP8266WiFi.h>

#define LED_MAX_BRIGHTNESS  120


// Replace with your network credentials
const char *ssid = "blah";
const char *password = "blah";

static uint16_t colorIdx=0;
static uint16_t ledBrightness=0;
static bool singleStepSwitch=0;
bool isEnabledColorBreathMode=false;

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

extern void ledArrayInit();
extern void setLedArrayProperties(uint16_t,uint16_t);
extern void runLedArraySimTest();
extern void colorBreathMode();

typedef void (*reIniCallbk)(uint16_t, uint16_t);
extern void reInitFromEep(reIniCallbk cbk) ;
extern void requestEepWrite(uint8_t col,uint8_t br,bool isImmediate);
extern void eepnit();
extern void task_EepHandler();



void setup()
{
   //Serial.begin(115200);
    // Connect to Wi-Fi network with SSID and password
   //Serial.print("Connecting to ");
   //Serial.println(ssid);
   
    eepnit();
    ledArrayInit();
    reInitFromEep(&setLedArrayProperties);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
       //Serial.print(".");
    }
    // Print local IP address and start web server
   //Serial.println("");
   //Serial.println("WiFi connected.");
   //Serial.println("IP address: ");
    //Serial.println(WiFi.localIP());
    String ip= WiFi.localIP().toString();
   //Serial.println(ip);
    server.begin();
  

}







void loop()
{

    WiFiClient client = server.available(); // Listen for incoming clients

    if (client)
    {                                  // If a new client connects,
       //Serial.println("New Client."); // print a message out in the serial port
        String currentLine = "";       // make a String to hold incoming data from the client
        while (client.connected())
        { // loop while the client's connected
            if (client.available())
            {                           // if there's bytes to read from the client,
                char c = client.read(); // read a byte, then
                //Serial.write(c);        // print it out the serial monitor
                header += c;
                if (c == '\n')
                { // if the byte is a newline character
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0)
                    {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();

                        // turns the GPIOs on and off
                        if (header.indexOf("GET /fwd") >= 0)
                        {
                           //Serial.println("FWD");
                           //Serial.println("fwd pwm:");
              
                            colorIdx++;
                            if(singleStepSwitch==1)
                            {
                              setLedArrayProperties(colorIdx,ledBrightness);
                              requestEepWrite(colorIdx,ledBrightness,false);
                            }
                           
                        }
                        else if (header.indexOf("GET /rev") >= 0)
                        {
                           //Serial.println("REVERSE");
                            colorIdx--;
                            if(singleStepSwitch==1)
                            {
                              setLedArrayProperties(colorIdx,ledBrightness);
                              requestEepWrite(colorIdx,ledBrightness,false);
                            }
                            
                           
                        }
                        else if (header.indexOf("GET /left") >= 0)
                        {
                           //Serial.println("LEFT TURN");
                           //runLedArraySimTest();
                           if(isEnabledColorBreathMode)
                           {
                              isEnabledColorBreathMode=false;
                           }
                           else
                           {
                              isEnabledColorBreathMode=true;
                           }

                            
                        }
                        else if (header.indexOf("GET /right") >= 0)
                        {
                           //Serial.println("RIGHT TURN");
                            if (singleStepSwitch==0)
                            {
                              singleStepSwitch=1;
                            }
                            else
                            {
                              singleStepSwitch=0;
                            }
                            
                        }
                        else if (header.indexOf("GET /stop") >= 0)
                        {
                           //Serial.println("Stop fired");
                            setLedArrayProperties(colorIdx,ledBrightness);
                            requestEepWrite(colorIdx,ledBrightness,true);

                           
                        }
                        else if (header.indexOf("slide") >= 0)
                        {
                           //Serial.println("----------Slider----------------");
                            // extract slider value from querystring
                            String val = header.substring(header.indexOf("slide") + 6, header.indexOf("slide") + 9);
                           //Serial.print("Header value string: ");
                           //Serial.println(val);

                           //Serial.print("Header value int");
                            int sliderVal = val.substring(0, val.length()).toInt();
                           //Serial.println(sliderVal);

                            ledBrightness = map(sliderVal, 0, 100, 0, LED_MAX_BRIGHTNESS);
                          
                           
                        }

                        // ----------------- UI ---------------------------------
                        client.println("<!DOCTYPE html>");
                        client.println("<html>");
                        client.println("<head>");
                        // client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                        //prevent zoom scroll
                        client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no\" />");
                       
                        client.println("<style>");
                        client.println("html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");

                        client.println(".button { background-color: #f0e92b; border: none;border-radius:5px; color: white; padding: 5px 10px;");
                        client.println("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}");
                        client.println(".button3 { position:relative;}");
                        client.println(".button2 {background-color: #3f4545;}");
                        client.println(".slidecontainer {width: 100%;}");
                        client.println(".slider {-webkit-appearance: none;width: 100%;height: 40px;background: #d3d3d3;outline: none;opacity: 0.7;-webkit-transition: .2s;transition: opacity .2s;}");
                        client.println(".slider:hover {opacity: 1;}");
                        client.println(".slider::-webkit-slider-thumb {-webkit-appearance: none;appearance: none;width: 25px;height: 40px;background: #4CAF50;cursor: pointer;}");
                        client.println("td {align=\"right\"}");
                        client.println(".slider::-moz-range-thumb {width: 25px;height: 40px;background: #4CAF50;cursor: pointer;}");
                        client.println("</style>");
                        client.println("</head>");
                        client.println("<body style=\"background-color:#b3ecff\">");
                        client.println("<h1>Node Control Pannel</h1>");
                        client.println("<table style=\" width:100%\">");
                        //client.println("<table >");
                        client.println("<tr><td></td><td><button class=\"button\" id=\"btnfwd\">Next</button></td><td></td></tr>");
                        client.println("<tr><td><button class=\"button\" id=\"btnlft\">LEFT</button></td>");
                        client.println("<td><button class=\"button\" id=\"btnstp\" style=\"background-color:#ff0000\">SET</button></td>");
                        client.println("<td><button class=\"button\" id=\"btnrgt\">RIGHT</button></td>");
                        client.println("<tr><td></td><td><button class=\"button\" id=\"btnrev\">Prev</button></td>");
                        client.println("<td></td></tr></table>");
                        client.println("<p>Slide to adjust brightness</p>");
                        client.println("<div class=\"slidecontainer\">");
                        client.println("<input type=\"range\" min=\"0\" max=\"100\" value=\"0\" class=\"slider\" id=\"myRange\">");
                        client.println("<p>Value: <span id=\"demo\"></span></p>");
                        client.println("</div>");
                        client.println("<script> ");
                        client.println("var slider = document.getElementById(\"myRange\");");
                         client.println("var output = document.getElementById(\"demo\");");
                         
                        client.println("var btnfwd = document.getElementById(\"btnfwd\");");
                        client.println("var btnrev = document.getElementById(\"btnrev\");");
                        client.println("var btnlft = document.getElementById(\"btnlft\");");
                        client.println("var btnrgt = document.getElementById(\"btnrgt\");");
                        client.println("var btnstp = document.getElementById(\"btnstp\");");

                       
                        client.println("output.innerHTML = slider.value;");
                        client.println("slider.oninput = function() {output.innerHTML = this.value;}");
                        client.println("slider.onmouseleave = function(){");
                        client.println("console.log(\"Slider event fired \");");
                        client.println("var xhttp = new XMLHttpRequest();");
                        String ip=WiFi.localIP().toString();
                        client.println("xhttp.open(\"GET\", \"http://"+ip+"/slide/\" + this.value  , true);");
                        client.println("xhttp.send();}");
                        
                        client.println("btnfwd.onclick = function(){ ");
                        client.println("var xhttp = new XMLHttpRequest(); ");
                        client.println("xhttp.open(\"GET\", \"http://"+ip+"/fwd\"  , true); ");
                        client.println("xhttp.send();} ");

                        client.println("btnrev.onclick = function(){ ");
                        client.println("var xhttp = new XMLHttpRequest(); ");
                        client.println("xhttp.open(\"GET\", \"http://"+ip+"/rev\"  , true); ");
                        client.println("xhttp.send();} ");

                        client.println("btnrgt.onclick = function(){ ");
                        client.println("var xhttp = new XMLHttpRequest(); ");
                        client.println("xhttp.open(\"GET\", \"http://"+ip+"/right\"  , true); ");
                        client.println("xhttp.send();} ");

                        client.println("btnlft.onclick = function(){ ");
                        client.println("var xhttp = new XMLHttpRequest(); ");
                        client.println("xhttp.open(\"GET\", \"http://"+ip+"/left\"  , true); ");
                        client.println("xhttp.send();} ");

                        client.println("btnstp.onclick = function(){ ");
                        client.println("var xhttp = new XMLHttpRequest(); ");
                        client.println("xhttp.open(\"GET\", \"http://"+ip+"/stop\"  , true); ");
                        client.println("xhttp.send();} ");
                        
                        client.println("slider.ontouchend = function(){");
                        client.println("console.log(\"Slider event fired \");");
                        client.println("var xhttp = new XMLHttpRequest();");
                        client.println("xhttp.open(\"GET\", \"http://"+ip+"/slide/\" + this.value  , true);");
                        client.println("xhttp.send();}</script>");
                        //client.println("}</script>");
                        client.println("</body></html>");

                        // The HTTP response ends with another blank line
                        client.println();
                        // Break out of the while loop
                        
                        break;
                    }
                    else
                    { // if you got a newline, then clear currentLine
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                {                     // if you got anything else but a carriage return character,
                    currentLine += c; // add it to the end of the currentLine
                }
            }
        }

        header = "";

       //Serial.println("Client disconnected.");
       //Serial.println("");
    }
    task_EepHandler();
    if(isEnabledColorBreathMode)
    {
      colorBreathMode();
    }
}
