#include <string>
#include <sstream>
#include <Thread.h>
#include <ThreadController.h>
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;

class ClockThread: public Thread
{
  unsigned int now;
  
  public:
  
    void setTime(unsigned int currentTime) {
      now = currentTime;
    }
    
    void run() {
      now++;
      runned();
    }
    
    unsigned int getTime() {
      return now;
    }


};

class SensorThread: public Thread
{
    int pin;
    double temperature;

    double thermister(int RawADC) {
      double temp;
      temp = log(((10240000 / RawADC) - 10000));
      temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * temp * temp)) * temp);
      temp = temp - 273.15;
      temp = (temp * 9.0) / 5.0 + 32;
      temp -= 12.8;
      return temp;
    }

  public:
    SensorThread(int p) : pin(p) {}

    double getTemperature() {
      return temperature;
    }
    int getValue(){
      return analogRead(pin);
      }

    // No, "run" cannot be anything...
    // Because Thread uses the method "run" to run threads,
    // we MUST overload this method here. using anything other
    // than "run" will not work properly...
    void run() {
      // Reads the analog pin, and saves it localy
      int value = analogRead(pin);
      double newtemp = thermister(value);
      if (temperature != 0){
        temperature = (temperature * 0.8) + (newtemp * 0.2);
      }else{
        temperature = newtemp;
        }
        runned();
    }
    
};

unsigned int getServerTime() {
  unsigned result = 0;
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    HTTPClient http;
    USE_SERIAL.print("[HTTP] begin...\n");
    http.begin("http://192.168.4.1:4567/time"); //HTTP
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      USE_SERIAL.printf("[HTTP] GET... code: % d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        USE_SERIAL.println(payload);
        result = payload.toInt();
      }
    } else {
      USE_SERIAL.printf("[HTTP] POST... failed, error: % s\n", http.errorToString(httpCode).c_str());
      result = 0;
    }

    http.end();
  }
  return result;
}

int deviceId = 1;
SensorThread analogThread(A0);
ClockThread clockThread;

class HTTPThread : public Thread {
    // callback for myThread
    void run() {
      Serial.print("Printing data to HTTP");
      Serial.println();
      Serial.print("The sensor is reading: ");
      Serial.println(analogThread.getTemperature());
      Serial.println(analogThread.getValue());

      if ((WiFiMulti.run() == WL_CONNECTED)) {
        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        char buf[1024];
        sprintf(buf, "http://192.168.4.1:4567/temp/%d/%d/%f", deviceId, clockThread.getTime(), analogThread.getTemperature());
        http.begin(buf); //HTTP

        USE_SERIAL.print("[HTTP] POST...\n");
        // start connection and send HTTP header
        int httpCode = http.POST("");

        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          USE_SERIAL.printf("[HTTP] POST... code: %d\n", httpCode);

          // file found at server
          if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            USE_SERIAL.println(payload);
          }
        } else {
          USE_SERIAL.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
      }
      runned();
    }
};

HTTPThread httpThread;

// ThreadController that will controll all threads
ThreadController controll = ThreadController();

void setup() {
  Serial.begin(9600);

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  clockThread.setTime(getServerTime());

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("NameOfNetwork", "AardvarkBadgerHedgehog");

  IPAddress addr = WiFi.localIP();
  deviceId = addr[3];
  
  // Configure my threads
  analogThread.setInterval(100);
  httpThread.setInterval(1000);
  clockThread.setInterval(1000);

  // Adds both threads to the controller
  controll.add(&clockThread);
  controll.add(&analogThread);
  delay(1000);
  controll.add(&httpThread);
}

void loop() {
  controll.run();
}

