#include <DHT.h>

#define DHTPIN 2      // DHT11 센서가 연결된 핀
#define DHTTYPE DHT11 // DHT11 센서 유형

DHT dht(DHTPIN, DHTTYPE);  // DHT 객체 선언

int relayFanPin = 9;       // 환풍기를 제어할 릴레이 핀
int relayPumpPin = 10;     // 펌프를 제어할 릴레이 핀
int LEDrelayPin = 11;      // LED 릴레이 핀

bool manualMode = false;   // 수동 모드 플래그 (false는 자동 모드, true는 수동 모드)
bool fanState = false;     // 환풍기 상태 저장 (true: ON, false: OFF)
bool pumpState = false;    // 펌프 상태 저장 (true: ON, false: OFF)

float setTemperature = 23.0;  // 초기 설정 온도
float setHumidity = 50.0;     // 초기 설정 습도

void setup() {
  Serial.begin(9600);        // 시리얼 통신 시작
  dht.begin();               // DHT 센서 시작

  pinMode(relayFanPin, OUTPUT);
  pinMode(relayPumpPin, OUTPUT);
  pinMode(LEDrelayPin, OUTPUT);

  // 릴레이는 LOW에서 작동하기 때문에 초기 상태는 OFF로 설정
  digitalWrite(relayFanPin, HIGH);
  digitalWrite(relayPumpPin, HIGH);
  digitalWrite(LEDrelayPin, HIGH);  // LED 릴레이 초기 OFF
}

void loop() {
  // 시리얼 통신을 통해 수동/자동 명령 및 ON/OFF 명령을 받음
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');  // 시리얼 데이터 수신

    // 수동/자동 모드 전환 명령
    if (input == "manual") {
      manualMode = true;
      Serial.println("수동 모드로 전환되었습니다.");
    } else if (input == "auto") {
      manualMode = false;
      Serial.println("자동 모드로 전환되었습니다.");
    }

    // 수동 모드에서 팬 ON/OFF
    if (manualMode) {
      if (input == "fan on") {
        fanState = true;
        digitalWrite(relayFanPin, LOW);  // 환풍기 ON
        Serial.println("환풍기 작동 중...");
      } else if (input == "fan off") {
        fanState = false;
        digitalWrite(relayFanPin, HIGH); // 환풍기 OFF
        Serial.println("환풍기 정지.");
      }

      // 수동 모드에서 펌프 ON/OFF
      if (input == "pump on") {
        pumpState = true;
        digitalWrite(relayPumpPin, HIGH);  // 펌프 ON
        Serial.println("펌프 작동 중...");
      } else if (input == "pump off") {
        pumpState = false;
        digitalWrite(relayPumpPin, LOW); // 펌프 OFF
        Serial.println("펌프 정지.");
      }
    }

    // 'led button' 명령을 받으면 LED 릴레이를 0.5초간 닫고 다시 열기
    if (input == "led button") {
      digitalWrite(LEDrelayPin, LOW);  // 릴레이 닫힘
      Serial.println("Relay is closed for 0.5 seconds.");
      delay(500);  // 0.5초 대기 (릴레이 닫힌 상태 유지)
      digitalWrite(LEDrelayPin, HIGH);  // 릴레이 다시 열림
      Serial.println("Relay is open again.");
    }
  }

  // 자동 모드일 때만 온도 및 습도 기반 제어
  if (!manualMode) {
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
      digitalWrite(relayPumpPin, HIGH);   // 릴레이 ON (펌프 작동)
      Serial.println("펌프 작동 중...");
    } else {
      digitalWrite(relayPumpPin, LOW);  // 릴레이 OFF (펌프 정지)
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
  }

  // 릴레이 핀 상태를 시리얼로 출력하여 확인
  Serial.print("relayFanPin 상태: ");
  Serial.println(digitalRead(relayFanPin));
  Serial.print("relayPumpPin 상태: ");
  Serial.println(digitalRead(relayPumpPin));
  Serial.print("LEDrelayPin 상태: ");
  Serial.println(digitalRead(LEDrelayPin));

  // 2초마다 센서 값 업데이트
  delay(2000);
}
