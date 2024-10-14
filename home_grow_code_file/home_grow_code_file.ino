#include <DHT.h>

#define DHTPIN 2      // DHT11 센서가 연결된 핀
#define DHTTYPE DHT11 // DHT11 센서 유형

DHT dht(DHTPIN, DHTTYPE);  // DHT 객체 선언

int relayFanPin = 9;       // 환풍기를 제어할 릴레이 핀
int relayPumpPin = 10;     // 펌프를 제어할 릴레이 핀

float setTemperature = 23.0;  // 초기 설정 온도
float setHumidity = 50.0;     // 초기 설정 습도

void setup() {
  Serial.begin(9600);        // 시리얼 통신 시작
  dht.begin();               // DHT 센서 시작

  pinMode(relayFanPin, OUTPUT);
  pinMode(relayPumpPin, OUTPUT);

  // 릴레이는 LOW에서 작동하기 때문에 초기 상태는 OFF로 설정
  digitalWrite(relayFanPin, HIGH);
  digitalWrite(relayPumpPin, HIGH);
}

void loop() {
  // 시리얼 통신을 통해 받은 데이터 처리
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');  // 시리얼 데이터 수신
    // 데이터 포맷 예시: "T23H50"
    if (data.startsWith("T")) {
      int tIndex = data.indexOf("T");
      int hIndex = data.indexOf("H");
      if (tIndex != -1 && hIndex != -1) {
        setTemperature = data.substring(tIndex + 1, hIndex).toFloat();  // 온도 값 추출
        setHumidity = data.substring(hIndex + 1).toFloat();  // 습도 값 추출
        Serial.print("설정 온도: ");
        Serial.print(setTemperature);
        Serial.print(" °C, 설정 습도: ");
        Serial.print(setHumidity);
        Serial.println(" %");
      }
    }
  }

  // DHT11 센서로부터 온도와 습도 값 읽기
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // 온도나 습도 값이 제대로 읽히지 않으면 에러 처리
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("DHT 센서를 읽는 중 오류 발생!");
    return;
  }

  // 시리얼 모니터에 온도와 습도 출력
  Serial.print("습도: ");
  Serial.print(humidity);
  Serial.print("%  ");
  Serial.print("온도: ");
  Serial.print(temperature);
  Serial.println("°C");

  // 습도가 설정한 값보다 낮으면 펌프 ON, 높으면 OFF
  if (humidity < setHumidity) {
    digitalWrite(relayPumpPin, LOW);   // 릴레이 ON (펌프 작동)
    Serial.println("펌프 작동 중...");
  } else {
    digitalWrite(relayPumpPin, HIGH);  // 릴레이 OFF (펌프 정지)
    Serial.println("펌프 정지.");
  }

  // 온도가 설정한 값보다 높으면 환풍기 ON, 낮으면 OFF
  if (temperature > setTemperature) {
    digitalWrite(relayFanPin, LOW);    // 릴레이 ON (환풍기 작동)
    Serial.println("환풍기 작동 중...");
  } else {
    digitalWrite(relayFanPin, HIGH);   // 릴레이 OFF (환풍기 정지)
    Serial.println("환풍기 정지.");
  }

  // 2초마다 센서 값 업데이트
  delay(2000);
}
