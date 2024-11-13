#include <DHT.h>
#include <EEPROM.h>

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

int relayFanPin = 9;
int relayPumpPin = 10;
int LEDrelayPin = 11;

bool manualMode = false;
bool fanState;
bool pumpState;

float setTemperature = 23.0;
float setHumidity = 50.0;

unsigned long lastPrintTime = 0;
const unsigned long printInterval = 2000;

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(relayFanPin, OUTPUT);
  pinMode(relayPumpPin, OUTPUT);
  pinMode(LEDrelayPin, OUTPUT);

  fanState = EEPROM.read(0);
  pumpState = EEPROM.read(1);

  digitalWrite(relayFanPin, fanState ? LOW : HIGH);
  digitalWrite(relayPumpPin, pumpState ? LOW : HIGH);
  digitalWrite(LEDrelayPin, HIGH);
}

void loop() {
  handleSerialInput(); // 수신 처리를 별도 함수로 분리
  handleSensorData();  // 센서 데이터 주기적 송신
}

// 수신 데이터 처리 함수
void handleSerialInput() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');

    // 수동/자동 모드 전환 명령
    if (input == "CMD:mode:manual") {
      manualMode = true;
      Serial.println("ACK");  // 명령 수신 확인 ACK 전송
      Serial.println("수동 모드로 전환되었습니다.");
    } else if (input == "CMD:mode:auto") {
      manualMode = false;
      Serial.println("ACK");  // 명령 수신 확인 ACK 전송
      Serial.println("자동 모드로 전환되었습니다.");
    }

    // 수동 모드에서 팬과 펌프 상태 조작
    if (manualMode) {
      if (input == "CMD:fan:1") {
        fanState = true;
        digitalWrite(relayFanPin, LOW);
        EEPROM.update(0, fanState);
        Serial.println("ACK");  // 명령 수신 확인 ACK 전송
        Serial.println("환풍기 작동 중...");
      } else if (input == "CMD:fan:0") {
        fanState = false;
        digitalWrite(relayFanPin, HIGH);
        EEPROM.update(0, fanState);
        Serial.println("ACK");  // 명령 수신 확인 ACK 전송
        Serial.println("환풍기 정지.");
      }

      if (input == "CMD:pump:1") {
        pumpState = true;
        digitalWrite(relayPumpPin, LOW);
        EEPROM.update(1, pumpState);
        Serial.println("ACK");  // 명령 수신 확인 ACK 전송
        Serial.println("펌프 작동 중...");
      } else if (input == "CMD:pump:0") {
        pumpState = false;
        digitalWrite(relayPumpPin, HIGH);
        EEPROM.update(1, pumpState);
        Serial.println("ACK");  // 명령 수신 확인 ACK 전송
        Serial.println("펌프 정지.");
      }

      if (input == "CMD:led:button") {
        digitalWrite(LEDrelayPin, LOW);
        delay(500);
        digitalWrite(LEDrelayPin, HIGH);
        Serial.println("ACK");  // 명령 수신 확인 ACK 전송
        Serial.println("Relay is open again.");
      }
    }

    // 설정 온도 및 습도 변경 명령
    if (input.startsWith("CMD:t")) {
      setTemperature = input.substring(5).toFloat();
      Serial.println("ACK");  // 명령 수신 확인 ACK 전송
      Serial.print("설정 온도가 ");
      Serial.print(setTemperature);
      Serial.println("°C로 변경되었습니다.");
    }

    if (input.startsWith("CMD:h")) {
      setHumidity = input.substring(5).toFloat();
      Serial.println("ACK");  // 명령 수신 확인 ACK 전송
      Serial.print("설정 습도가 ");
      Serial.print(setHumidity);
      Serial.println("%로 변경되었습니다.");
    }
  }
}

// 주기적으로 센서 데이터를 송신하는 함수
void handleSensorData() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastPrintTime >= printInterval) {
    lastPrintTime = currentMillis;

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("DHT 센서를 읽는 중 오류 발생!");
      return;
    }

    //Serial.print("SENSOR:현재 습도: ");
    Serial.print(humidity);
    //Serial.print("% | 설정 습도: ");
   // Serial.print(setHumidity);
    //Serial.println("%");

    //Serial.print("SENSOR:현재 온도: ");
    Serial.print(temperature);
    //Serial.print("°C | 설정 온도: ");
    //Serial.println(setTemperature);

    if (!manualMode) {
      if (humidity < setHumidity) {
        digitalWrite(relayPumpPin, HIGH);
        Serial.println("펌프 작동 중...");
      } else {
        digitalWrite(relayPumpPin, LOW);
        Serial.println("펌프 정지.");
      }

      if (temperature > setTemperature) {
        digitalWrite(relayFanPin, LOW);
        Serial.println("환풍기 작동 중...");
      } else {
        digitalWrite(relayFanPin, HIGH);
        Serial.println("환풍기 정지.");
      }
    } else {
      Serial.print("펌프 상태: ");
      Serial.println(digitalRead(relayPumpPin) == LOW ? "정지" : "작동 중");
      Serial.print("환풍기 상태: ");
      Serial.println(digitalRead(relayFanPin) == LOW ? "정지" : "작동 중");
    }
  }
}
