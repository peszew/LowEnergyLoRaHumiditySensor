#include <dht.h>

// Define the pin where the DHT11 is connected
#define DHTPIN 2  // Digital pin 2

dht DHT; // Create an instance of the dht class

void setup() {
  Serial.begin(9600);
  Serial.println("DHT11 Sensor Reading");
}

void loop() {
  // Read data from the DHT11 sensor
  DHT.read11(DHTPIN);

  // Print temperature and humidity readings
  Serial.print("Humidity: ");
  Serial.print(DHT.humidity, 1); // Humidity value with 1 decimal point
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(DHT.temperature, 1); // Temperature value with 1 decimal point
  Serial.println(" °C");

  // Wait 2 seconds before taking another reading
  delay(2000);
}
