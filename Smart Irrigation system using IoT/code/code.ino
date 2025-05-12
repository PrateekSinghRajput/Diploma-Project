#include <ESP8266WiFi.h> 
#define BLYNK_PRINT Serial
#include "DHTesp.h"
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
SimpleTimer timer;
char auth[] = "7EOx0kYMNxdCyhNVFlT5llYcp-DeS7RI";

#define DHTpin 0   //D3 of NodeMCU is GPIO14
DHTesp dht;
WiFiClient client;
WiFiServer server(80);


int Raw       = A0;      //Analog channel A0 as used to measure temperature
int threshold = 16;      //Nodemcu digital pin water sensor read-GPIO16---D0 of NodeMCU
int Relay = 13;       // GPIO13---D7 of NodeMCU--Motor connection
int WiFiStrength = 0;
int value;

const char* ssid = "Redmi";
const char* password = "partik123";

const char *host ="api.thingspeak.com";//api.thingspeak.com
String apiKey = "G3ACNARGLITZU2TB"; 

//To connect to IFTTT
String MakerIFTTT_Key ;
String MakerIFTTT_Event;
char *append_str(char *here, String s) {  int i=0; while (*here++ = s[i]){i++;};return here-1;}
char *append_ul(char *here, unsigned long u) { char buf[20]; return append_str(here, ultoa(u, buf, 10));}
char post_rqst[256];char *p;
char *content_length_here;
char *json_start;
int compi;

void setup(void)
{ 
  Serial.begin(115200);
  Blynk.begin(auth, ssid, password);
  delay(10);
  dht.setup(DHTpin, DHTesp::DHT11); //for DHT11 Connect DHT sensor to GPIO 17
  pinMode(threshold,INPUT_PULLUP); //Pin#13 as output-Activate pullup at pin 13
  pinMode(Relay, OUTPUT);       //D7 as output
  digitalWrite(Relay, LOW);     //Deactivate Relay
  // Connect to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
  Serial.println("");
  Serial.println("WiFi connected");
 
  server.begin();
  Serial.println("Server started");
  // Print the IP address on serial monitor
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");    
  Serial.print(WiFi.localIP());
  Serial.println("/");
  Serial.println((WiFi.localIP().toString()));
  Serial.println(".");
  Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)\tHeatIndex (C)\tMoisture(%)\tWifi-Strength\tMotor-Pump");
}

void loop() {
 //timer.run(); // Initiates SimpleTimer
 Blynk.run();
 int state = digitalRead(13);
 float h =0.0;  //Humidity level
 float t =0.0;  //Temperature in celcius 
 float f =0.0;  //Temperature in fahrenheit
 double analogValue = 0.0;
 double analogVolts = 0.0;
 unsigned long timeHolder = 0;
 //delay(dht.getMinimumSamplingPeriod());
 h = dht.getHumidity();    //Read humidity level
 t = dht.getTemperature(); //Read temperature in celcius
 f = (t * 1.8 ) + 32;        //Temperature converted to Fahrenheit
 WiFiStrength = WiFi.RSSI(); 
 analogValue = analogRead(A0); // read the analog signal
 analogVolts = 100-(analogValue * 100) / 1024;
 Blynk.virtualWrite(V0, t);
 Blynk.virtualWrite(V1, h);
 WidgetLED led1(V2);
 Blynk.virtualWrite(V3, analogVolts);
 int chartValue = analogVolts; //(analogValue * 100) / 510;
 //chartValue = 100 - chartValue;
 
 Serial.print(dht.getStatusString());
 Serial.print("\t");
 Serial.print(h, 1);
 Serial.print("\t\t");
 Serial.print(t, 1);
 Serial.print("\t\t");
 Serial.print(dht.toFahrenheit(t), 1);
 Serial.print("\t");
 Serial.print(dht.computeHeatIndex(t, h, false), 1);
 Serial.print("\t\t");
 Serial.print(analogVolts, 1);
 Serial.print("\t\t");
 Serial.print(WiFiStrength);
 Serial.print("dBm");
 Serial.print("\t\t");
 if(state == HIGH) 
   Serial.print("Operational");
   else{
   if(state == LOW) 
    Serial.print("Halt");
   }
    Serial.println(value);
    Serial.println(state);  
  
//delay(5000);
  WiFiClient client = server.available();   // Check if a client has connected
  String request = client.readStringUntil('\r');
  if (digitalRead(14)== LOW){
  if (digitalRead(2)== LOW) {
    Serial.println("webpage off");
    if (analogVolts<=20){
    digitalWrite(Relay, HIGH);
    value = HIGH;
    }
    if (analogVolts>=20){
    digitalWrite(Relay, LOW);
    value = LOW;
    }}} 
  Serial.println("new client");
  //while(!client.available()){       // Wait until the client sends some data
    //delay(1);
  //}
      // Read the first line of the request
  Serial.println(request);
  Serial.println("***********************************************************************************************************************");
  Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)\tHeatIndex (C)\tMoisture(%)\tWifi-Strength\tMotor-Pump");
  client.flush(); 
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  //
  
  client.println("<body bgcolor=white>");
  client.println("<h1 align=center>TEAM-OMNIPRESENT</h1><br><br>");
  client.println("<h1 align=center>Smart Irrigation</h1><br><br>");
  client.print("<h2 style=\"size:12px;\"> Webpage Controls:</h2>");
  // show some data on the webpage and the guage
  client.println("<table><tr><td>");
  client.print("WiFi Signal Strength: ");
  client.println(WiFiStrength);
  client.println("dBm<br>");
  //client.print("Moisture(%): ");
  //client.println(analogValue);
  client.print("<br>Moisture(%): ");
  client.println(analogVolts);
  
  client.print("<br>Temperature in Celsius =");
  client.println(t);
  client.println("<br>");
  client.print("Temperature in Fahrenheit =");
  client.println(f);
  client.println("<br>");
  client.print("Humidity =");
  client.println(h);
  client.print(" %");   
  client.println("<br>");  
  client.println(" <head>");
  client.println("<meta http-equiv=\"refresh\" content=\"5\">");
  client.println(" <script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>");
  client.println("  <script type=\"text/javascript\">");
  client.println("    google.charts.load('current', {'packages':['gauge']});");
  client.println("    google.charts.setOnLoadCallback(drawChart);");
  client.println("   function drawChart() {");
  client.println("      var data = google.visualization.arrayToDataTable([ ");
  client.println("        ['Label', 'Value'], ");
  client.print("        ['Moisture',  ");
  client.print(chartValue);
  client.println(" ], ");
  client.println("       ]); ");
  
  // setup the google chart options here
  client.println("    var options = {");
  client.println("      width: 900, height: 400,");
  client.println("      redFrom: 0, redTo: 25,");
  client.println("      yellowFrom: 25, yellowTo: 75,");
  client.println("      greenFrom: 75, greenTo: 100,");
  client.println("       minorTicks: 5");
  client.println("    };");
  client.println("   var chart = new google.visualization.Gauge(document.getElementById('chart_div'));");
  client.println("  chart.draw(data, options);");
  client.println("  setInterval(function() {");
  client.print("  data.setValue(0, 1, ");
  client.print(chartValue);
  client.println("    );");
  client.println("    chart.draw(data, options);");
  client.println("    }, 13000);");
  client.println("  }");
  client.println(" </script>");
  client.println("  </head>");
  client.println("  <body>");
  client.println("</td><td>");
  // below is the google chart html
  client.println("<div id=\"chart_div\" style=\"width: 800px; height: 320px;\"></div>");
  client.println("</td></tr></table>");
  delay(1);

  // Match the request
  if (digitalRead(14)== HIGH){
    if (digitalRead(15)== LOW)
      digitalWrite(Relay, LOW);
    if (digitalRead(15)== HIGH)
    digitalWrite(Relay, HIGH);
    }
  
  if(digitalRead(14)== LOW){
  if(digitalRead(2)== HIGH){
    if (request.indexOf("/Relay=STOP") != -1){
        digitalWrite(Relay, LOW);
        value = LOW;
    }
    else{
      if (request.indexOf("/Relay=ON") != -1) // If less mositure in soil start the motor otherwise stop
      {
        digitalWrite(Relay, HIGH);
        value = HIGH;
      }
      else{
      if (request.indexOf("/Relay=OFF") != -1){
          if (analogVolts<=50)// If less mositure in soil start the motor otherwise stop
          {
            digitalWrite(Relay, HIGH);
            value = HIGH;
          }
          else{
          if (analogVolts>=50){
            digitalWrite(Relay, LOW);
            value = LOW;
            }}}}}}}
      
      
      //client.println("<br><a href=\"/Up=ON\"\"><button>Refresh</button></a><br />"); 
  client.println("<a href=\"/Relay=ON\"\"><button>Pump On </button></a>");
  client.println("<a href=\"/Relay=OFF\"\"><button>Pump Automatic </button></a><br />");
 // client.println("<a href=\"/Relay=START\"\"><button>Pump Function Start </button></a><br />");
  client.println("<a href=\"/Relay=STOP\"\"><button>Pump Stop </button></a><br />");
   
  
 /*if(digitalRead(threshold)==HIGH)
  { // Read digital output of soil sensor
  client.println("Threshold Reached = Rain detected / Moisture exceeded / Water detected");
  }*/
  client.println("<br><br>");
  if(state == HIGH) {
   client.println("Motor/Pump Operational");
   Blynk.virtualWrite(V4, HIGH);
   led1.on();
  }
  else{
  if(state == LOW)  {
    client.print("Motor/Pump at Halt");
    Blynk.virtualWrite(V4, LOW);
    led1.off();
  }}
    client.println("</html>");
  delay(10);
  Serial.print("");
}
