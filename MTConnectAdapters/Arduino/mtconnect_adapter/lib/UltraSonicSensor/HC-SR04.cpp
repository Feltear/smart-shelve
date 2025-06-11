#include <HC-SR04.h>

int HCSR04::getDistance() {
    // Clear the sensor
    // Set the trigger pin to LOW for 2 microseconds
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    // Trigger
    // Send a 10 microsecond pulse to the trigger pin
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Read the echo pin to get the duration of the pulse
    this->data.duration = pulseIn(echoPin, HIGH);

    // Calculate the distance in centimeters (speed of sound is approximately 343 m/s)
    this->data.distance = this->data.duration * 0.034 / 2; // Divide by 2 because the signal travels to the object and back

    return this->data.distance;
}