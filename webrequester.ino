#include <SoftwareSerial.h>

SoftwareSerial esp8266(2, 3);
#define speed8266 9600
#define CH_PD 4
#define DEBUG true

// Remote site information
const char http_site[] = "checkip.amazonaws.com";
const int http_port = 80;

void setup()
{
  esp8266.begin (speed8266);
  Serial.begin(9600);
  reset8266(); // Pin CH_PD needs a reset before start communication

  sendData("AT+RST\r\n", 2000, DEBUG); // reset
  sendData("AT+CWJAP=\"SSID\",\"PASSWORD\"\r\n", 2000, DEBUG); //Connect network
  delay(3000);
  sendData("AT+CWMODE=1\r\n", 1000, DEBUG);
  sendData("AT+CIFSR\r\n", 1000, DEBUG); // Show IP Adress
  //sendData("AT+CIPMUX=1\r\n", 1000, DEBUG); // Multiple conexions
  //sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG); // start comm port 80
}

void loop()
{
  Serial.println("starting");
  WebRequest("icanhazip.com/index.php");   
  delay(30000); 
}

//web request needs to be sent without the http for now, https still needs some working
void WebRequest(String request){
 //find the dividing marker between domain and path
     int slash = request.indexOf('/');
     
     //grab the domain
     String domain;
     if(slash>0){  
       domain = request.substring(0,slash);
     }else{
       domain = request;
     }

     //get the path
     String path;
     if(slash>0){  
       path = request.substring(slash);   
     }else{
       path = "/";          
     }
     
     //output domain and path to verify
     Serial.println("domain: |" + domain + "|");
     Serial.println("path: |" + path + "|");     
     
     //create start command
     String startcommand = "AT+CIPSTART=\"TCP\",\"64.182.208.183\", 80"; //443 is HTTPS, still to do
     
    
     
     esp8266.println(startcommand);
     Serial.println(startcommand);

      if(esp8266.find("Linked")) {
        Serial.println("Linked!");
      }
     
     //test for a start error
     if(esp8266.find("Error")){
       esp8266.println("error on start");
       return;
     }
     
     //create the request command
     //String sendcommand = "GET http://"+ domain + path + " HTTP/1.0\r\n\r\n\r\n";//works for most cases

     String sendcommand="GET http://icanhazip.com/index.php HTTP/1.1\r\n"
      "User-Agent: ESP8266\r\n"
      "Host: icanhazip.com\r\n"
      "Connection: close\r\n\r\n";
     
     Serial.print(sendcommand);
     
     //send 
     esp8266.print("AT+CIPSEND=");
     esp8266.println(sendcommand.length());
     
     //debug the command
     //Serial.print("AT+CIPSEND=");
     //Serial.println(sendcommand.length());
     
     //delay(5000);
     if(esp8266.find(">"))
     {
       Serial.println(">");
       Serial.print("enviou os pacotes");
     }else
     {
       esp8266.println("AT+CIPCLOSE");
       Serial.println("connect timeout");
       delay(1000);
       return;
     }
     
     //Serial.print(getcommand);
     esp8266.print(sendcommand);
     Serial.print("enviou o comando");
  
  String response = "";
     long int time = millis();
    while ( (time + 3000) > millis())
    {
      while (esp8266.available())
      {
        // The esp has data so display its output to the serial window
        char c = esp8266.read(); // read the next character.
        response += c;
      }
    }
    if (true)
    {
      Serial.print(response);
    }
  
}

/*************************************************/
// Send AT commands to module
String sendData(String command, const int timeout, boolean debug)
{
  String response = "";
  esp8266.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
  return response;
}
/*************************************************/
// Reset funtion to accept communication
void reset8266 ()
{
  pinMode(CH_PD, OUTPUT);
  digitalWrite(CH_PD, LOW);
  delay(300);
  digitalWrite(CH_PD, HIGH);
}
