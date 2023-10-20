// Arduino Wheel Motion with FreeRTOS using semaphore and mutexes

#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#define ENA 9
#define ENB 10
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7

// Task handles
TaskHandle_t motorATask;
TaskHandle_t motorBTask;

// Semaphores for motor control
SemaphoreHandle_t motorASemaphore;
SemaphoreHandle_t motorBSemaphore;

// Function prototypes for task functions
void motorATaskFunction(void* parameter);
void motorBTaskFunction(void* parameter);

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

  // Create tasks for each motor
  xTaskCreate(motorATaskFunction, "MotorATask", 100, NULL, 1, &motorATask);
  xTaskCreate(motorBTaskFunction, "MotorBTask", 100, NULL, 1, &motorBTask);

  // Start the FreeRTOS scheduler
  vTaskStartScheduler();
}

void loop() {
  // This function should be empty when using FreeRTOS.
  // All code execution should be done in tasks.
}

void motorATaskFunction(void* parameter) {
  (void)parameter; // Unused

  while (1) {
    // Request access to the motor A
    if (xSemaphoreTake(motorASemaphore, portMAX_DELAY) == pdTRUE) {
      analogWrite(ENA, 255);
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);

      // Release the semaphore to allow other tasks to access the motor
      xSemaphoreGive(motorASemaphore);
    }

    // You can add a delay here if needed
    vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 milliseconds
  }
}

void motorBTaskFunction(void* parameter) {
  (void)parameter; // Unused

  while (1) {
    // Request access to the motor B
    if (xSemaphoreTake(motorBSemaphore, portMAX_DELAY) == pdTRUE) {
      analogWrite(ENB, 255);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);

      // Release the semaphore to allow other tasks to access the motor
      xSemaphoreGive(motorBSemaphore);
    }

    // You can add a delay here if needed
    vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 milliseconds
  }
}
