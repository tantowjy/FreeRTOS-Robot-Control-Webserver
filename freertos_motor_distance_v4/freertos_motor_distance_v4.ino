#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#define ENA 9
#define ENB 10
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7
#define TRIG1 3
#define ECHO1 2
#define TRIG2 8
#define ECHO2 11

// Task handles
TaskHandle_t motorATask;
TaskHandle_t motorBTask;
TaskHandle_t ultrasonicTask1;
TaskHandle_t ultrasonicTask2;

// Semaphores for motor control
SemaphoreHandle_t motorASemaphore;
SemaphoreHandle_t motorBSemaphore;
SemaphoreHandle_t ultrasonicMutex;

// Queue for motor speeds
QueueHandle_t motorSpeedQueue;

// Shared variables
long distance1 = 0;
long distance2 = 0;

// Motor speed constants
const int forwardSpeed = 150;  // Adjust the value as needed
const int stopSpeed = 0;

// Function prototypes for task functions
void motorATaskFunction(void* parameter);
void motorBTaskFunction(void* parameter);
void ultrasonicTaskFunction(void* parameter);

void setup() {
  Serial.begin(9600);

  // Initialize the pins
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Create semaphores for motor control
  motorASemaphore = xSemaphoreCreateMutex();
  motorBSemaphore = xSemaphoreCreateMutex();
  ultrasonicMutex = xSemaphoreCreateMutex();

  // Create Queue for motor speeds
  motorSpeedQueue = xQueueCreate(1, sizeof(int));

  // Create tasks for each motor
  xTaskCreate(motorATaskFunction, "MotorATask", 100, NULL, 1, &motorATask);
  xTaskCreate(motorBTaskFunction, "MotorBTask", 100, NULL, 1, &motorBTask);

  // Create tasks for ultrasonic sensors
  xTaskCreate(ultrasonicTaskFunction, "UltrasonicTask1", 100, (void*)1, 2, &ultrasonicTask1);
  xTaskCreate(ultrasonicTaskFunction, "UltrasonicTask2", 100, (void*)2, 2, &ultrasonicTask2);

  // Start the FreeRTOS scheduler
  vTaskStartScheduler();
}

void loop() {
  // This function should be empty when using FreeRTOS.
  // All code execution should be done in tasks.
}

void motorATaskFunction(void* parameter) {
  (void)parameter; // Unused
  int motorSpeed;
  while (1) {
    // Receive motor speed from the queue
    if (xQueueReceive(motorSpeedQueue, &motorSpeed, portMAX_DELAY) == pdTRUE) {
      // Request access to the motor A
      if (xSemaphoreTake(motorASemaphore, portMAX_DELAY) == pdTRUE) {
        if (motorSpeed > 0) {
          analogWrite(ENA, motorSpeed);
          digitalWrite(IN1, HIGH);
          digitalWrite(IN2, LOW);
        } else {
          analogWrite(ENA, 0); // Stop the motor
        }

        // Release the semaphore to allow other tasks to access the motor
        xSemaphoreGive(motorASemaphore);
      }
    }
    // You can add a delay here if needed
    vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 milliseconds
  }
}

void motorBTaskFunction(void* parameter) {
  (void)parameter; // Unused
  int motorSpeed;
  while (1) {
    // Receive motor speed from the queue
    if (xQueueReceive(motorSpeedQueue, &motorSpeed, portMAX_DELAY) == pdTRUE) {
      // Request access to the motor B
      if (xSemaphoreTake(motorBSemaphore, portMAX_DELAY) == pdTRUE) {
        if (motorSpeed > 0) {
          analogWrite(ENB, motorSpeed);
          digitalWrite(IN3, LOW);
          digitalWrite(IN4, HIGH);
        } else {
          analogWrite(ENB, 0); // Stop the motor
        }

        // Release the semaphore to allow other tasks to access the motor
        xSemaphoreGive(motorBSemaphore);
      }
    }
    // You can add a delay here if needed
    vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 milliseconds
  }
}

void ultrasonicTaskFunction(void* parameter) {
  int ultrasonicNumber = (int)parameter;
  while (1) {
    // Request access to the ultrasonic sensor data
    if (xSemaphoreTake(ultrasonicMutex, portMAX_DELAY) == pdTRUE) {
      long distance = (ultrasonicNumber == 1) ? distance1 : distance2;
      xSemaphoreGive(ultrasonicMutex);

      // Check distance and control motors accordingly
      if (ultrasonicNumber == 2 && distance >= 0 && distance <= 40) {
        // Request access to motors for both tasks
        if (xSemaphoreTake(motorASemaphore, portMAX_DELAY) == pdTRUE &&
            xSemaphoreTake(motorBSemaphore, portMAX_DELAY) == pdTRUE) {
          // Set motor speeds to move forward
          xQueueSend(motorSpeedQueue, &forwardSpeed, portMAX_DELAY);

          // Release semaphores to allow other tasks to access the motors
          xSemaphoreGive(motorASemaphore);
          xSemaphoreGive(motorBSemaphore);
        }
      } else if (ultrasonicNumber == 1 && distance >= 0 && distance <= 40) {
        // Request access to motors for both tasks
        if (xSemaphoreTake(motorASemaphore, portMAX_DELAY) == pdTRUE &&
            xSemaphoreTake(motorBSemaphore, portMAX_DELAY) == pdTRUE) {
          // Set motor speeds to stop
          xQueueSend(motorSpeedQueue, &stopSpeed, portMAX_DELAY);

          // Release semaphores to allow other tasks to access the motors
          xSemaphoreGive(motorASemaphore);
          xSemaphoreGive(motorBSemaphore);
        }
      }
    }
    // You can add a delay here if needed
    vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 milliseconds
  }
}
