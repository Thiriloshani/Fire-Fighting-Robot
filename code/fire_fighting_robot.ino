#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#define LEFT_FLAME   34
#define RIGHT_FLAME  35
#define MIDDLE_FLAME 39

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

String account_sid = YOUR_TWILIO_ACCOUNT_SID"";
String auth_token  = "YOUR_TWILIO_AUTH_TOKEN";

String from = "whatsapp:YOUR_TWILIO_WHATSAPP_NUMBER";
String to   = "whatsapp:YOUR_PHONE_NUMBER";

bool alertSent = false;   // 🔥 IMPORTANT

String urlencode(String str);

void setup() {
  Serial.begin(115200);

  pinMode(LEFT_FLAME, INPUT);
  pinMode(RIGHT_FLAME, INPUT);
  pinMode(MIDDLE_FLAME, INPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi CONNECTED");
}

void loop() {

  int leftValue   = analogRead(LEFT_FLAME);
  int rightValue  = analogRead(RIGHT_FLAME);
  int middleValue = analogRead(MIDDLE_FLAME);

  int threshold = 2000;

  bool fireDetected = (leftValue < threshold || 
                       middleValue < threshold || 
                       rightValue < threshold);

  // 🔥 PRINT VALUES
  Serial.print("L:");
  Serial.print(leftValue);
  Serial.print(" M:");
  Serial.print(middleValue);
  Serial.print(" R:");
  Serial.println(rightValue);

  // 🚀 SEND WHATSAPP ONLY ONCE
  if (fireDetected && !alertSent) {
    Serial.println("FIRE DETECTED → Sending WhatsApp");

    sendAlert("Fire Detected!");

    alertSent = true;
  }

  // 🔄 Reset when no fire
  if (!fireDetected) {
    alertSent = false;
  }

  delay(500);
}

void sendAlert(String message) {

  if (WiFi.status() == WL_CONNECTED) {

    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;

    String url = "https://api.twilio.com/2010-04-01/Accounts/" + account_sid + "/Messages.json";

    http.begin(client, url);
    http.setAuthorization(account_sid.c_str(), auth_token.c_str());
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // ✅ IMPORTANT FIX FOR +
   String body = "From=whatsapp:%2B14155238886&To=whatsapp:%2B917358112271&Body=" + message;

    int httpResponseCode = http.POST(body);

    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode);

    String response = http.getString();
    Serial.println(response);

    http.end();
  }
}

// 🔧 ENCODING FUNCTION
String urlencode(String str) {
  String encoded = "";
  char c;
  char code0;
  char code1;

  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);

    if (isalnum(c)) {
      encoded += c;
    } else {
      encoded += '%';
      code0 = (c >> 4) & 0xF;
      code1 = (c & 0xF);
      encoded += (code0 > 9) ? (code0 + 'A' - 10) : (code0 + '0');
      encoded += (code1 > 9) ? (code1 + 'A' - 10) : (code1 + '0');
    }
  }
  return encoded;
}
