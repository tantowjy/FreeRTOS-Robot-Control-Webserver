#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#define echoPin 2
#define trigPin 3
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7
#define ENA 9
#define ENB 10

#define speedOff 0
#define speedNormal 60

long distance, duration;

SemaphoreHandle_t distanceMutex;
SemaphoreHandle_t distanceSemaphore;

void setup() {
  // Initialize FreeRTOS
  distanceMutex = xSemaphoreCreateMutex();
  distanceSemaphore = xSemaphoreCreateBinary();
  xTaskCreate(distanceTask, "DistanceTask", 100, NULL, 1, NULL);
  xTaskCreate(motorControlTask, "MotorControlTask", 100, NULL, 2, NULL);
  vTaskStartScheduler();
}

void distanceTask(void* pvParameters) {
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);

  for (;;) {
    // Distance measurement code
    xSemaphoreTake(distanceMutex, portMAX_DELAY); // Lock the distance variable
    distance = 0; // Clear distance
    xSemaphoreGive(distanceMutex); // Unlock the distance variable

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);

    xSemaphoreTake(distanceMutex, portMAX_DELAY); // Lock the distance variable
    distance = duration / 58.2; // Update distance
    xSemaphoreGive(distanceMutex); // Unlock the distance variable

    xSemaphoreGive(distanceSemaphore); // Signal motorControlTask

    vTaskDelay(100); // Delay for 100ms
  }
}

void motorControlTask(void* pvParameters) {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  for (;;) {
    if (xSemaphoreTake(distanceSemaphore, portMAX_DELAY) == pdTRUE) {
      xSemaphoreTake(distanceMutex, portMAX_DELAY); // Lock the distance variable
      if (distance > 30) {
        motorAOn();
        motorBOn();
      } else {
        motorAOff();
        motorBOff();
      }
      xSemaphoreGive(distanceMutex); // Unlock the distance variable
    }

    vTaskDelay(100); // Delay for 100ms
  }
}

void motorAOn() {
  analogWrite(ENA, speedNormal);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}

void motorAOff() {
  analogWrite(ENA, speedOff);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

void motorBOn() {
  analogWrite(ENB, speedNormal);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void motorBOff() {
  analogWrite(ENB, speedOff);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void loop() {
  // Empty, as all tasks are managed by FreeRTOS
}
