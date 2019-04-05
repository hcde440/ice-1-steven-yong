/*A sketch to get the ESP8266 on the network and connect to some open services via HTTP to
 * get our external IP address and (approximate) geolocative information in the getGeo()
 * function. To do this we will connect to http://freegeoip.net/json/, an endpoint which
 * requires our external IP address after the last slash in the endpoint to return location
 * data, thus http://freegeoip.net/json/XXX.XXX.XXX.XXX
 * 
 * This sketch also introduces the flexible type definition struct, which allows us to define
 * more complex data structures to make receiving larger data sets a bit cleaner/clearer.
 * 
 * jeg 2017
 * 
 * updated to new API format for Geolocation data from ipistack.com
 * brc 2019
*/

#include <ESP8266WiFi.h>                                          //Include the WiFi library for the microcontroller.
#include <ESP8266HTTPClient.h>                                    //Include the client library for the microcontroller.
#include <ArduinoJson.h>                                          //Include the library to parse JSON.

const char* ssid = "YONG_2.4GHz";                                 //Create a constant character array for the WiFi name.
const char* pass = "4254028948";                                  //Create a constant character array for the WiFi password.
const char* geo_Key = "f5d77941089c38c4d23213dfe2b43837";         //Create a constant character array for the ipstack key.
const char* weather_Key = "9fe9f73f127da032fcf505a41c6f91dc";     //Create a constant character array for the OpenWeatherMap key.

typedef struct {                                                  //Create a struct type.
  String ip;                                                      //Put a string called ip in the struct.
  String cc;                                                      //Put a string called cc in the struct.
  String cn;                                                      //Put a string called cn in the struct.
  String rc;                                                      //Put a string called rc in the struct.
  String rn;                                                      //Put a string called rn in the struct.
  String cy;                                                      //Put a string called cy in the struct.
  String ln;                                                      //Put a string called ln in the struct.
  String lt;                                                      //Put a string called lt in the struct.
} GeoData;                                                        //Name the struct GeoData.

GeoData location;                                                 //Create a GeoData variable called location.

typedef struct {                                                  //Create a struct type.
  String temp;                                                    //Put a string called temp in the struct.
  String humidity;                                                //Put a string called humidity in the struct.
  String ws;                                                      //Put a string called ws in the struct.
  String wd;                                                      //Put a string called wd in the struct.
  String cc;                                                      //Put a string called cc in the struct.
} MetData;                                                        //Name the struct MetData.

MetData weather;                                                  //Create a MetData variable called weather.

void setup() {
  Serial.begin(115200);                                           //Initialize the serial monitor at a rate of 115200 baud.
  delay(10);                                                      //Delay by 10 milliseconds.
  Serial.print("This board is running: ");                        //Print this in the serial monitor.
  Serial.println(F(__FILE__));                                    //Print the file name in the serial monitor.
  Serial.print("Compiled: ");                                     //Print this in the serial monitor.
  Serial.println(F(__DATE__ " " __TIME__));                       //Print the date and time that the file was uploaded.
  
  Serial.print("Connecting to "); Serial.println(ssid);           //Print this in the serial monitor.

  WiFi.mode(WIFI_STA);                                            //Connect to the WiFi in station mode.
  WiFi.begin(ssid, pass);                                         //Connect to the WiFi using the constant character arrays previously created.

  while (WiFi.status() != WL_CONNECTED) {                         //If the WiFi does not connect do this.
    delay(500);                                                   //Delay by 500 milliseconds.
    Serial.print(".");                                            //Print this in the serial monitor.
  }

  Serial.println(); Serial.println("WiFi connected"); Serial.println();                             //Print this in the serial monitor.
  Serial.print("Your ESP has been assigned the internal IP address ");                              //Print this in the serial monitor.
  Serial.println(WiFi.localIP());                                                                   //Print the local IP address in the serial monitor.
  
  getGeo();                                                                                         //Call the getGeo function.

  Serial.println("Your external IP address is " + location.ip);                                     //Retrieve the external IP address and print this in the serial monitor.
  Serial.print("Your ESP is currently in " + location.cn + " (" + location.cc + "),");              //Retrieve the country and country code, and print this in the serial monitor.
  Serial.println(" in or near " + location.cy + ", " + location.rc + ".");                          //Retrieve the city and region code, and print this in the serial monitor.
  Serial.println("and located at (roughly) ");                                                      //Print this in the serial monitor.
  Serial.println(location.lt + " latitude by " + location.ln + " longitude.");                      //Retrieve the latitude and longitude, and print this in the serial monitor.

  getMet();                                                                                         //Call the getMet function.
  
  Serial.println();                                                                                 //Print this in the serial monitor.
  Serial.println("Temperature: " + weather.temp + "°F");                                            //Retrieve the temperature and print this in the serial monitor.
  Serial.println("Humidity: " + weather.humidity + "%");                                            //Retrieve the humidity and print this in the serial monitor.
  Serial.println("Wind Speed: " + weather.ws + " mph");                                             //Retrieve the wind speed and print this in the serial monitor.
  Serial.println("Wind Direction: " + weather.wd + "°");                                            //Retrieve the wind direction and print this in the serial monitor.
  Serial.println("Cloud Coverage: " + weather.cc + "%");                                            //Retrieve the cloud coverage and print this in the serial monitor.

}

void loop() {
}

String getIP() {                                                                          //Define the getIp function.
  HTTPClient theClient;                                                                   //Create a mini browser on the microcontroller.
  String ipAddress;                                                                       //Create a string variable called ipAddress.

  theClient.begin("http://api.ipify.org/?format=json");                                   //Open this link with the mini browser.
  int httpCode = theClient.GET();                                                         //Retrieve the HTTP code.

  if (httpCode > 0) {                                                                     //If the HTTP code is greater than 0, do this.
    if (httpCode == 200) {                                                                //If the HTTP code is 200, do this.

      DynamicJsonBuffer jsonBuffer;                                                       //Create a dynamic JSON buffer.

      String payload = theClient.getString();                                             //Store what the mini browser gives back as one big string.
      JsonObject& root = jsonBuffer.parse(payload);                                       //Parse the string.
      ipAddress = root["ip"].as<String>();                                                //Retrieve the IP address and convert it into a string.

    } else {                                                                              //If the HTTP code is not 200, do this.
      Serial.println("Something went wrong with connecting to the endpoint.");            //Print this in the serial monitor.
      return "error";                                                                     //Return an informational string for errors.
    }
  }
  return ipAddress;                                                                       //Return the IP address.
}

void getGeo() {                                                                           //Define the getGeo function.
  String ipAddress = getIP();                                                             //Create a string and assign it to the string returned by the getIP function.
  HTTPClient theClient;                                                                   //Create a mini browser on the microcontroller.
  Serial.println("Making HTTP request");                                                  //Print this in the serial monitor.
  theClient.begin("http://api.ipstack.com/" + ipAddress + "?access_key=" + geo_Key);      //Open this link in the mini browser.
  int httpCode = theClient.GET();                                                         //Retrieve the HTTP code.

  if (httpCode > 0) {                                                                     //If the HTTP code is greater than 0, do this.
    if (httpCode == 200) {                                                                //If the HTTP code is 200, do this.
      Serial.println("Received HTTP payload.");                                           //Print this in the serial monitor.
      DynamicJsonBuffer jsonBuffer;                                                       //Create a dynamic JSON buffer.
      String payload = theClient.getString();                                             //Store what the mini browser gives back as one big string.
      Serial.println("Parsing...");                                                       //Print this in the serial monitor.
      JsonObject& root = jsonBuffer.parse(payload);                                       //Parse the string.

      if (!root.success()) {                                                              //If the parsing was not successful, do this.
        Serial.println("parseObject() failed");                                           //Print this in the serial monitor.
        Serial.println(payload);                                                          //Print the unparsed string in the serial monitor.
        return;                                                                           //Return nothing.
      }

      location.ip = root["ip"].as<String>();                                              //Cast the IP address as a string and put it into the struct.
      location.cc = root["country_code"].as<String>();                                    //Cast the country code as a string and put it into the struct.
      location.cn = root["country_name"].as<String>();                                    //Cast the country name as a string and put it into the struct.
      location.rc = root["region_code"].as<String>();                                     //Cast the region code as a string and put it into the struct.
      location.rn = root["region_name"].as<String>();                                     //Cast the region name as a string and put it into the struct.
      location.cy = root["city"].as<String>();                                            //Cast the city as a string and put it into the struct.
      location.lt = root["latitude"].as<String>();                                        //Cast the latitude as a string and put it into the struct.
      location.ln = root["longitude"].as<String>();                                       //Cast the longitude as a string and put it into the struct.
      
    } else {                                                                              //If the HTTP code is not 200, do this.
      Serial.println("Something went wrong with connecting to the endpoint.");            //Print this in the serial monitor.
    }
  }
}

void getMet() {                                                                           //Define the getMet function.
  HTTPClient theClient;                                                                   //Create a mini browser on the microcontroller.
  Serial.println("Making HTTP request");                                                  //Print this in the serial monitor.
  theClient.begin("http://api.openweathermap.org/data/2.5/weather?q=" + location.cy + "&units=imperial" + "&APPID=" + weather_Key);       //Open this link in the mini browser.
  int httpCode = theClient.GET();                                                         //Retrieve the HTTP code.

  if (httpCode > 0) {                                                                     //If the HTTP code is greater than 0, do this.
    if (httpCode == 200) {                                                                //If the HTTP code is 200, do this.
      Serial.println("Received HTTP payload.");                                           //Print this in the serial monitor.
      DynamicJsonBuffer jsonBuffer;                                                       //Create a dynamic JSON buffer.
      String payload = theClient.getString();                                             //Store what the mini browser gives back as one big string.
      Serial.println("Parsing...");                                                       //Print this in the serial monitor.
      JsonObject& root = jsonBuffer.parse(payload);                                       //Parse the string.

      if (!root.success()) {                                                              //If the parsing was not successful, do this.
        Serial.println("parseObject() failed");                                           //Print this in the serial monitor.
        Serial.println(payload);                                                          //Print the unparsed string in the serial monitor.
        return;                                                                           //Return nothing.
      }

      weather.temp = root["main"]["temp"].as<String>();                                   //Cast the temperature as a string and put it into the struct.
      weather.humidity = root["main"]["humidity"].as<String>();                           //Cast the humidity as a string and put it into the struct.
      weather.ws = root["wind"]["speed"].as<String>();                                    //Cast the wind speed as a string and put it into the struct.
      weather.wd = root["wind"]["deg"].as<String>();                                      //Cast the wind direction as a string and put it into the struct.
      weather.cc = root["clouds"]["all"].as<String>();                                    //Cast the cloud coverage as a string and put it into the struct.
      
    } else {                                                                              //If the HTTP code is not 200, do this.
      Serial.println("Something went wrong with connecting to the endpoint.");            //Print this in the serial monitor.
    }
  }
}
