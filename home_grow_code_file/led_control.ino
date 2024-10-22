int relayPin = 11;  // 릴레이가 연결된 핀 번호

void setup() {
  pinMode(relayPin, OUTPUT);  // 릴레이 제어 핀을 출력 모드로 설정
  Serial.begin(9600);  // 시리얼 통신 시작
  digitalWrite(relayPin, HIGH);  // 릴레이를 처음에는 OFF 상태로 설정 (열림)
}

void loop() {
  // 시리얼 통신을 통해 명령을 받음
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');  // 시리얼로 입력된 명령을 받음

    // 'led on' 명령을 받으면 릴레이를 닫고 LED 켜기
    if (command == "led on") {
      digitalWrite(relayPin, LOW);  // 릴레이 닫힘 (LED 켜짐)
      Serial.println("LED is ON. Relay is closed.");
    }
    // 'led off' 명령을 받으면 릴레이를 열고 LED 끄기
    else if (command == "led off") {
      digitalWrite(relayPin, HIGH);  // 릴레이 열림 (LED 꺼짐)
      Serial.println("LED is OFF. Relay is open.");
    }
  }
}
