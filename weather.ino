#include <ESP8266HTTPClient.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <String.h>

void refreshing();
void get_image();
void disp_temp_forecast();
void get_geo_loc();
void disp_wind();
void disp_humidity();
void draw( const unsigned char forecast[]);



Adafruit_SSD1306 display(128, 32, &Wire, 4);
const char* ssid = "test";
const char* password = "46213228"; 
HTTPClient http;
WiFiClient client;
int status = WL_IDLE_STATUS;
int temp =0,humi;
float wind;
String Forecast="Not Set";
String Weather_response="";
String zip="",City="",State="";
String payload="";
void setup ()
 {
     Wire.begin(0,2);
     display.begin(SSD1306_SWITCHCAPVCC, 0x3C);    
     display.clearDisplay(); 
     display.display();       
     Serial.begin(9600);
     while (WiFi.status() != WL_CONNECTED)
      {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid); 
        WiFi.mode(WIFI_STA);       
        WiFi.begin(ssid, password);
        delay(5000);
      }

     
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor((128/2)-48,10);
    display.print("getting location");
    display.display(); 
    get_geo_loc();
      
     
 }
void get_geo_loc()
{
    String ip="49.206.5.63";
    String c;
    http.begin("http://api.ipify.org/");
    Serial.println("connected to ip server");
    int httpCode = http.GET();
    payload = http.getString();
    ip=payload;
    Serial.println(payload);
    http.end();
    String api_key="29dde4da0ce172bf6a3ecc39e3b25b3f";
    http.begin("http://api.ipstack.com/"+ip+"?access_key="+api_key);
    Serial.println("connected to geo server");
    httpCode = http.GET();
    payload = http.getString();
    zip=payload.substring(payload.indexOf("zip")+6,payload.indexOf(",",(payload.indexOf("zip")+8))-1);
    Serial.println(payload);
    http.end();
} 
void loop()
{   
    Serial.println("loop");
    int n=0;
    refreshing();
    while(n<3)
    {disp_temp_forecast();     
    delay(15000);
    disp_humidity();
    delay(15000);
    disp_wind();
    delay(15000);
    get_geo_loc();
    n++;}      
}
void refreshing()
{
   Serial.println("Getting Weather");
   display.clearDisplay();
   display.setTextColor(WHITE);
   display.setTextSize(1);
   display.setCursor(128/4,0);
   display.print("refreshing");
   delay(500);
   display.display();
   if (client.connect("api.openweathermap.org", 80)) 
    {
      Serial.println("connected to server");
      // Make a HTTP request:
      client.println("GET http://api.openweathermap.org/data/2.5/weather?zip="+zip+",in&appid=ac06c25dc52a0e049bae365f0d9ab1f3");
      client.println("Host: api.openweathermap.org");
      client.println("Connection: close");
      client.println();
    }
    String c ;  
    int flag=0 ;  
    while(flag==0)
    {
       if (client.available())
         {
            flag=1;
            c = client.readString();            
            Weather_response=c;
            Serial.println(c);
            
         }
    }
  
    if (!client.connected())
    {
        Serial.println();
        Serial.println("disconnecting from server.");
        client.stop();
    }
     City=payload.substring(payload.indexOf("city")+7,payload.indexOf(",",(payload.indexOf("city")+9))-1);
     State=payload.substring(payload.indexOf("region_code")+14,payload.indexOf(",",(payload.indexOf("region_code")+14))-1);
     temp=(Weather_response.substring(Weather_response.indexOf("temp")+6,Weather_response.indexOf(".",(Weather_response.indexOf("temp")+8)))).toInt();
     Forecast=Weather_response.substring(Weather_response.indexOf("main")+7,Weather_response.indexOf(",",(Weather_response.indexOf("main")+8))-1);
     wind=(Weather_response.substring(Weather_response.indexOf("speed")+7,Weather_response.indexOf(",",(Weather_response.indexOf("speed")+7)))).toFloat();
     humi=(Weather_response.substring(Weather_response.indexOf("humidity")+10,Weather_response.indexOf(",",(Weather_response.indexOf("humidity")+12)))).toInt();
     Serial.println(String(temp));
     
  
}
void disp_humidity()
{
    Serial.println("humidity");
    display.clearDisplay();
     display.setTextSize(2);     
     display.setCursor((128/2)+20,15);
     display.print(humi);     
     display.setTextSize(0.5); 
     display.setCursor((128/2)-((City.length()*3+State.length()*3)),0);
     display.print(City+","+State);     
     display.setCursor((128/2)+46,15);
     display.print("%");
     display.setCursor(15,(32/2)-2);
     display.setTextSize(1); 
     display.print("Humidity");
     display.display();

}
void disp_wind()
{
     Serial.println("wind");
     display.clearDisplay();
     display.setTextSize(0.5); 
     display.setTextSize(2);     
     display.setCursor((128/2)+10,15);
     display.printf("%.1f",wind);
     display.setTextSize(1);          
     display.setCursor((128/2)+46,20);
     display.print("M/s");
     display.setCursor((128/2)-((City.length()*3+State.length()*3)),0);
     display.print(City+","+State);
     display.setCursor(4,(32/2)+5); 
     display.print("Wind Speed");
     display.display();
}
void disp_temp_forecast()
{
     Serial.println("temp & forecast");
     display.clearDisplay();
     display.setTextColor(WHITE);  
     display.setTextSize(2);     
     display.setCursor((128/2)+14,15);
     display.print(temp-273);
     display.drawCircle((128/2)+40,15,2,WHITE);     
     display.setCursor((128/2)+46,15);
     display.print("C");
     display.setTextSize(0.5); 
     display.setCursor((128/2)-((City.length()*3+State.length()*3)),0);
     display.print(City+","+State);
     delay(300);
     get_image(); 
     display.display();    
}
void get_image()
{
     
    if (Forecast.equals("Clouds")||Forecast.equals("Cloudy"))
      {
        Forecast="Cloudy";
         const unsigned char Clouds []  = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf0, 0x00, 0x00, 0x00, 
  0xff, 0xfe, 0x00, 0x00, 0x01, 0xff, 0xff, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xe0, 0x00, 0x7f, 0xff, 
  0xff, 0xfe, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x80, 0xff, 0xff, 0xff, 
  0xff, 0x80, 0x7f, 0xff, 0xff, 0xff, 0x00, 0x3f, 0xff, 0xff, 0xfe, 0x00, 0x03, 0xff, 0xe0, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
         draw(Clouds);

       
      }
      if (Forecast.equals("Clear"))
      {

         const unsigned char Clear[]={0x00, 0x00, 0x81, 0x00, 0x00, 0x00, 0x40, 0x82, 0x00, 0x00, 0x00, 0x60, 0x04, 0x00, 0x00, 0x00, 
  0x07, 0xf0, 0x00, 0x00, 0x04, 0x1f, 0xfc, 0x30, 0x00, 0x07, 0x3f, 0xfe, 0x40, 0x00, 0x01, 0x7f, 
  0xff, 0x00, 0x00, 0x00, 0x7f, 0xff, 0x80, 0x00, 0x00, 0xff, 0xff, 0x80, 0x00, 0x3c, 0xff, 0xff, 
  0xbe, 0x00, 0x00, 0xff, 0xff, 0x80, 0x00, 0x00, 0xff, 0xff, 0x80, 0x00, 0x00, 0x7f, 0xff, 0x00, 
  0x00, 0x00, 0xbf, 0xfe, 0x80, 0x00, 0x01, 0x9f, 0xfc, 0x40, 0x00, 0x02, 0x07, 0xf0, 0x20, 0x00};
         draw(Clear);

      }
      if (Forecast.equals("Partially Cloudy"))
      {

         const unsigned char par[]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x00, 
 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x0f, 
  0xe0, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x0f, 0xf0, 0x1f, 
  0xf0, 0x00, 0x3f, 0xfe, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x80, 0xff, 0xff, 0xff, 0xff, 
  0x80, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
         draw(par);
      }
      if (Forecast.equals("Rain"))
      {

         const unsigned char rain[]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x00, 
 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x0f, 
  0xe0, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x0f, 0xf0, 0x1f, 
  0xf0, 0x00, 0x3f, 0xfe, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x80, 0xff, 0xff, 0xff, 0xff, 
  0x80, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
      
         draw(rain);
      }
    
    
   display.setCursor(20,32-8);
   display.setTextSize(1); 
   display.print(Forecast);

}
void draw( const unsigned char forecast[])
{
   display.drawBitmap(
    ((display.width()  - 33 ) / 4)-5,
    8,
    forecast, 33, 16, 1);
}



