int LEDrelayPin = 11;  // 릴레이가 연결된 핀 번호

void setup() {
  pinMode(LEDrelayPin, OUTPUT);  // 릴레이 제어 핀을 출력 모드로 설정
  Serial.begin(9600);  // 시리얼 통신 시작
  digitalWrite(LEDrelayPin, HIGH);  // 릴레이를 처음에는 OFF 상태로 설정 (열림)
}

void loop() {
  // 시리얼 통신을 통해 명령을 받음
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');  // 시리얼로 입력된 명령을 받음

    // 'led button' 명령을 받으면 릴레이를 0.5초간 닫고 다시 열기
    if (command == "led button") {
      digitalWrite(LEDrelayPin, LOW);  // 릴레이 닫힘
      Serial.println("Relay is closed for 0.5 seconds.");
      delay(500);  // 0.5초 대기 (릴레이 닫힌 상태 유지)
      digitalWrite(LEDrelayPin, HIGH);  // 릴레이 다시 열림
      Serial.println("Relay is open again.");
    }
  }
}
