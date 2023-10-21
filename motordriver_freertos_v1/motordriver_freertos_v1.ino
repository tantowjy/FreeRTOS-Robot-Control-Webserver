#include <Arduino_FreeRTOS.h>

#define ENA 9
#define ENB 10
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7

// Task handles
TaskHandle_t motorATask;
TaskHandle_t motorBTask;

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
    analogWrite(ENA, 255);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    // You can add a delay here if needed
    vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 milliseconds
  }
}

void motorBTaskFunction(void* parameter) {
  (void)parameter; // Unused

  while (1) {
    analogWrite(ENB, 255);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);

    // You can add a delay here if needed
    vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 milliseconds
  }
}
