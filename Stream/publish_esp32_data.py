import paho.mqtt.client as mqtt
import time
import random

MOSQUITTO_IP_ADDRESS = "localhost"
mqtt_broker = MOSQUITTO_IP_ADDRESS
mqtt_port = 1883
mqtt_topic = "esp32/data"

connected = False  # Track connection state

def on_connect(client, userdata, flags, reason_code, properties):
    global connected
    if reason_code == 0:
        print("Connected to MQTT Broker!")
        connected = True
    else:
        print(f"Connection failed: {reason_code}")

client = mqtt.Client(
    client_id="PythonPublisher",
    callback_api_version=mqtt.CallbackAPIVersion.VERSION2
)
client.on_connect = on_connect

try:
    client.connect(mqtt_broker, mqtt_port)
    client.loop_start()  # Start background network loop

    # Wait for connection (max 5 seconds)
    start_time = time.time()
    while not connected and (time.time() - start_time < 5):
        time.sleep(0.1)

    if connected:
        try:
            while True:
                sensor_value = random.uniform(0, 100)
                client.publish(mqtt_topic, str(sensor_value))
                print(f"Published: {sensor_value}")
                time.sleep(2)
        except KeyboardInterrupt:
            print("Publisher stopped.")
    else:
        print("Failed to connect to MQTT broker")

except Exception as e:
    print(f"Error: {e}")

finally:
    client.loop_stop()
    client.disconnect()
    print("Client disconnected")