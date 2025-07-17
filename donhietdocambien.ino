#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// Thông tin WiFi
const char* ssid = "TOTOLINK_269c";
const char* password = "1234567890";

// Khai báo cảm biến DHT
#define DHTPIN 4         // Chân GPIO kết nối DHT (ví dụ chân D4)
#define DHTTYPE DHT11    // DHT11 hoặc DHT22 tùy loại bạn dùng
#define LED_PIN 2
DHT dht(DHTPIN, DHTTYPE);

// URL API
const char* serverName = "http://api.benhvienhuyencuchi.vn/api/theodoinhietdodoam";

// Thời gian gửi dữ liệu (đơn vị ms)
unsigned long lastTime = 0;
unsigned long timerDelay = 600000;

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  WiFi.begin(ssid, password);
  Serial.print("Đang kết nối WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nKết nối WiFi thành công!");
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    if (WiFi.status() == WL_CONNECTED) {
      float nhietdo = dht.readTemperature();
      float doam = dht.readHumidity();

      if (isnan(nhietdo) || isnan(doam)) {
        Serial.println("Không đọc được dữ liệu từ cảm biến DHT!");
        return;
      }

      // In ra Serial
      Serial.printf("Nhiệt độ: %.1f °C - Độ ẩm: %.1f %%\n", nhietdo, doam);

      // Tạo JSON body
      String jsonData = "{\"nhietdo\":\"" + String(nhietdo, 1) + "\",\"doam\":\"" + String(doam, 1) + "\"}";

      // Gửi HTTP POST
      HTTPClient http;
      http.begin(serverName);
      http.addHeader("Content-Type", "application/json");

      int httpResponseCode = http.POST(jsonData);

      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("Phản hồi server: " + response);
      } else {
        Serial.printf("Lỗi gửi POST: %s\n", http.errorToString(httpResponseCode).c_str());
      }

      http.end();
      // Chớp đèn D2
      digitalWrite(LED_PIN, HIGH);
      delay(100); // Chớp 100ms
      digitalWrite(LED_PIN, LOW);
    } else {
      Serial.println("Mất kết nối WiFi");
    }

    lastTime = millis();
  }
}
