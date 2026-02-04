#define SYH_PIN A0  // Analog pin connected to the SYH-2R sensor
#define SERIES_RESISTOR 10000 // Series resistor value in ohms (10kΩ)

void setup() {
  Serial.begin(9600);
  Serial.println("SYH-2R Humidity Sensor Reading");
}

void loop() {
  // Read analog value from the sensor
  int analogValue = analogRead(SYH_PIN);
  
  // Convert analog value to voltage (assuming 5V system)
  float voltage = analogValue * (5.0 / 1023.0);
  
  // Convert voltage back to resistance of the SYH-2R sensor
  float sensorResistance = (SERIES_RESISTOR * (5.0 - voltage)) / voltage;

  // Map the resistance to humidity (based on the datasheet curve)
  // Replace this formula with the one provided in the sensor's datasheet
  float humidity = map(sensorResistance, 2000, 10000, 10, 90); // Example mapping
  
  // Print the readings
  Serial.print("Analog Value: ");
  Serial.print(analogValue);
  Serial.print("\tVoltage: ");
  Serial.print(voltage);
  Serial.print(" V\tResistance: ");
  Serial.print(sensorResistance);
  Serial.print(" ohms\tHumidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  delay(2000); // Wait 2 seconds before the next reading
}
