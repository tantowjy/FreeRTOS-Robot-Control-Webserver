#define echoPin 2
#define trigPin 3
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7
#define ENA 9
#define ENB 10
#define Speed 60
long duration, distance;

void setup(){
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void motorAOn(){
  analogWrite(ENA, Speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}

void motorAOff(){
  analogWrite(ENA, Speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

void motorBOn(){
  analogWrite(ENB, Speed);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void motorBOff(){
  analogWrite(ENB, Speed);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void loop(){
  // Clear the trigger pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Send a 10µs pulse to trigger the sensor
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the duration of the echo pulse
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in centimeters
  distance = duration/58.2;

  if (distance > 30){
    motorAOn();
    motorBOn();
  }
  else {
    motorAOff();
    motorBOff();
  }
  delay(100);
}