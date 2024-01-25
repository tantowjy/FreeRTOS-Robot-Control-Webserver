#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <semphr.h>

// Arduino pin
#define echoPin 2
#define trigPin 3
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7
#define ENA 9
#define ENB 10

// Motor speed with variables
#define speedOff 0
#define speedNormal 60

long distance, duration;

// Task handles
TaskHandle_t distanceTask;
TaskHandle_t motorAOnTask;
TaskHandle_t motorBOnTask;
TaskHandle_t motorAOffTask;
TaskHandle_t motorBOffTask;

// Semaphore for distance and motor control
SemaphoreHandle_t distanceMutex;
SemaphoreHandle_t distanceSemaphore;
SemaphoreHandle_t motorAOnSemaphore;
SemaphoreHandle_t motorBOnSemaphore;
SemaphoreHandle_t motorAOffSemaphore;
SemaphoreHandle_t motorBOffSemaphore;

void setup() {
  // Initialize the pins
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Create semaphores for distance
  distanceMutex = xSemaphoreCreateMutex();
  distanceSemaphore = xSemaphoreCreateBinary();

  // Create semaphores for motor control
  motorAOnSemaphore = xSemaphoreCreateMutex();
  motorBOnSemaphore = xSemaphoreCreateMutex();
  motorAOffSemaphore = xSemaphoreCreateMutex();
  motorBOffSemaphore = xSemaphoreCreateMutex();

  // Create tasks for distance
  xTaskCreate(distanceTaskFunction, "DistanceTask", 100, NULL, 1, &distanceTask);

  // Create tasks for each motor
  xTaskCreate(motorAOnTaskFunction, "MotorAOnTask", 100, NULL, 3, &motorAOnTask);
  xTaskCreate(motorBOnTaskFunction, "MotorBOnTask", 100, NULL, 4, &motorBOnTask);
  xTaskCreate(motorAOffTaskFunction, "MotorAOffTask", 100, NULL, 3, &motorAOffTask);
  xTaskCreate(motorBOffTaskFunction, "MotorBOffTask", 100, NULL, 4, &motorBOffTask);

  xTaskCreate(motorControlTask, "MotorControlTask", 100, NULL, 2, NULL);
  vTaskStartScheduler();
}

void distanceTaskFunction(void* pvParameters) {
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

void motorAOnTaskFunction(void* parameter) {
  (void)parameter; // Unused

  while (1) {
    // Control motor A
    analogWrite(ENA, speedNormal);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    // You can add a delay here if needed
    vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 milliseconds
  }
}

void motorBOnTaskFunction(void* parameter) {
  (void)parameter; // Unused

  while (1) {
    // Control motor B
    analogWrite(ENB, speedNormal);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);

    // You can add a delay here if needed
    vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 milliseconds
  }
}

void motorAOffTaskFunction(void* parameter) {
  (void)parameter; // Unused

  while (1) {
    // Control motor A
    analogWrite(ENA, speedOff);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

    // You can add a delay here if needed
    vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 milliseconds
  }
}

void motorBOffTaskFunction(void* parameter) {
  (void)parameter; // Unused

  while (1) {
    // Control motor B
    analogWrite(ENB, speedOff);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);

    // You can add a delay here if needed
    vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 milliseconds
  }
}

void motorControlTask(void* pvParameters) { 
  for (;;) {
    if (xSemaphoreTake(distanceSemaphore, portMAX_DELAY) == pdTRUE) {
      xSemaphoreTake(distanceMutex, portMAX_DELAY); // Lock the distance variable
      if (distance > 30) {
        // Turn on motor A and B
        xSemaphoreTake(motorAOnSemaphore, portMAX_DELAY);
        xSemaphoreTake(motorBOnSemaphore, portMAX_DELAY);
        xSemaphoreGive(motorAOffSemaphore); // Give the semaphores back
        xSemaphoreGive(motorBOffSemaphore);
      } else {
        // Turn off motor A and B
        xSemaphoreTake(motorAOffSemaphore, portMAX_DELAY);
        xSemaphoreTake(motorBOffSemaphore, portMAX_DELAY);
        xSemaphoreGive(motorAOnSemaphore); // Give the semaphores back
        xSemaphoreGive(motorBOnSemaphore);
      }
      xSemaphoreGive(distanceMutex); // Unlock the distance variable
    }

    vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100ms
  }
}

void loop() {
  // Empty, as all tasks are managed by FreeRTOS
}
