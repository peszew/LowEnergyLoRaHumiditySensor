import spidev
import time
import paho.mqtt.client as mqtt

spi = spidev.SpiDev()
spi.open(0, 0)  # Bus 0, Device 0
spi.max_speed_hz = 500000


def lora_init():
   
    spi.xfer2([0x01, 0x80])  
    spi.xfer2([0x06, 0x6C])  
    spi.xfer2([0x1D, 0x72])  
    spi.xfer2([0x1E, 0x74])  
    spi.xfer2([0x0D, 0x00])  
    spi.xfer2([0x0F, 0x00])  
    print("LoRa initialized")


def lora_receive():
    
    spi.xfer2([0x00])  
    data = spi.readbytes(32) 
    return data


def decode_message(data):
    try:
        message = data.decode('utf-8')
        if ',' in message:
            temperature, humidity = message.split(',')
            temperature = float(temperature)
            humidity = float(humidity)
            return temperature, humidity
    except Exception as e:
        print(f"Error decoding message: {e}")
    return None, None


def publish_to_mqtt(temperature, humidity):
    client = mqtt.Client("LoRa_Receiver")
    client.connect("localhost", 1883, 60) 
    client.loop_start()

    if temperature is not None and humidity is not None:
        client.publish("home/lora/temperature", temperature)
        client.publish("home/lora/humidity", humidity)
        print(f"Published: Temperature={temperature}, Humidity={humidity}")

    client.loop_stop()
    client.disconnect()


def main():
    lora_init()
    while True:
        raw_data = lora_receive()
        temperature, humidity = decode_message(bytes(raw_data))
        if temperature is not None and humidity is not None:
            publish_to_mqtt(temperature, humidity)
        time.sleep(1)

if __name__ == "__main__":
    main()
