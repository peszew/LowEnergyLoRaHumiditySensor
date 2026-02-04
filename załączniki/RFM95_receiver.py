import spidev
import time
import paho.mqtt.client as mqtt

# Konfiguracja SPI
spi = spidev.SpiDev()
spi.open(0, 0)  # Bus 0, Device 0
spi.max_speed_hz = 500000

# Funkcja do inicjalizacji modułu LoRa (RFM95)
def lora_init():
    # Ustawienia RFM95
    spi.xfer2([0x01, 0x81])  
    spi.xfer2([0x06, 0xD9])  
    spi.xfer2([0x07, 0x00])
    spi.xfer2([0x08, 0x00])
    spi.xfer2([0x1D, 0x72])  
    spi.xfer2([0x1E, 0x74])  
    spi.xfer2([0x0D, 0x00])  
    spi.xfer2([0x0F, 0x80])  
    spi.xfer2([0x01, 0x85])  
    print("RFM95 initialized")


def lora_receive():
   
    irq_flags = spi.xfer2([0x12, 0x00])[1]
    if irq_flags & 0x40:  # RX_DONE flag
        
        spi.xfer2([0x13, 0x00])  
        length = spi.readbytes(1)[0]

        
        spi.xfer2([0x0D, 0x00])

       
        data = spi.readbytes(length)

        
        spi.xfer2([0x12, 0xFF])  
        return data
    return None


def decode_message(data):
    try:
        message = data.decode('utf-8')  
        if 'Temp' in message and 'Hum' in message and 'Batt' in message:
            parts = message.split(',')
            temperature = float(parts[0].split(':')[1].strip())  
            humidity = float(parts[1].split(':')[1].strip())     
            battery = int(parts[2].split(':')[1].strip())        
            return temperature, humidity, battery
    except Exception as e:
        print(f"Error decoding message: {e}")
    return None, None, None


def publish_to_mqtt(temperature, humidity, battery):
    client = mqtt.Client("LoRa_Receiver")
    client.connect("localhost", 1883, 60) 
    client.loop_start()

    if temperature is not None and humidity is not None and battery is not None:
        client.publish("home/lora/temperature", temperature)
        client.publish("home/lora/humidity", humidity)
        client.publish("home/lora/battery", battery)
        print(f"Published: Temp={temperature}, Hum={humidity}, Batt={battery}")

    client.loop_stop()
    client.disconnect()


def main():
    lora_init()
    while True:
        raw_data = lora_receive()
        if raw_data:
            temperature, humidity, battery = decode_message(bytes(raw_data))
            if temperature is not None and humidity is not None and battery is not None:
                publish_to_mqtt(temperature, humidity, battery)
        time.sleep(1)

if __name__ == "__main__":
    main()
