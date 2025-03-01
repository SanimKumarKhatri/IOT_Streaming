import paho.mqtt.client as mqtt
import time
import logging
# Configuration
MOSQUITTO_BROKER = "mqtt.eclipseprojects.io"
MQTT_PORT = 1883
MQTT_TOPIC = "mpu6050_iot/all_data"
CLIENT_ID = "PythonSubscriber"
RECONNECT_DELAY = 5  # in seconds in case of failure
#.............................
# Configure logging
logging.basicConfig(
    format='%(asctime)s %(levelname)s: %(message)s',
    level=logging.INFO
)
class MQTTSubscriber:
    def __init__(self):
        self.connected = False
        self.client = mqtt.Client(
            client_id=CLIENT_ID,
            callback_api_version=mqtt.CallbackAPIVersion.VERSION2
        )
        self._setup_callbacks()
        #.....mongoDB setup................
        # self.mongoDBInstance=CloudMongoDB()
        
    def _setup_callbacks(self):
        """Configure MQTT client callbacks"""
        self.client.on_connect = self._on_connect
        self.client.on_disconnect = self._on_disconnect
        self.client.on_message = self._on_message
        self.client.on_log = self._on_log

    def _on_connect(self, client, userdata, flags, reason_code, properties):
        """Callback for when the client connects to the broker"""
        if reason_code == 0:
            logging.info("Connected to MQTT Broker")
            self.connected = True
            client.subscribe(MQTT_TOPIC, qos=1)
        else:
            logging.error(f"Connection failed: {reason_code}")
            self.connected = False

    def _on_disconnect(self, client, userdata, disconnect_flags, reason_code, properties):
        """Callback for when the client disconnects from the broker"""
        logging.warning(f"Disconnected from broker (reason: {reason_code})")
        self.connected = False
        self._reconnect()

    def _on_message(self, client, userdata, message):
        """Callback for when a message is received"""
        try:
            payload = message.payload.decode()
            parts = payload.split(',')
            datetime = parts[0]
            ax, ay, az, gx, gy, gz, temp = map(float, parts[1:]) 
            sensor_document = {
                    "time": datetime,
                    "ax": ax,
                    "ay": ay,
                    "az": az,
                    "gx": gx,
                    "gy": gy,
                    "gz": gz,
                    "temperature": temp
                }
            logging.info(f"Data->{sensor_document} Received!")
        except UnicodeDecodeError:
            logging.error("Received malformed message payload")

    def _on_log(self, client, userdata, level, buf):
        """Optional logging callback (useful for debugging)"""
        logging.debug(f"MQTT Log: {buf}")

    def _reconnect(self):
        """Handle reconnection attempts with backoff"""
        while not self.connected:
            logging.info(f"Attempting reconnection in {RECONNECT_DELAY} seconds...")
            time.sleep(RECONNECT_DELAY)
            try:
                self.client.reconnect()
                return
            except Exception as e:
                logging.error(f"Reconnection failed: {str(e)}")

    def connect(self):
        """Establish initial connection to the broker"""
        try:
            self.client.connect(MOSQUITTO_BROKER, MQTT_PORT)
            self.client.loop_start()
            
            # Wait for initial connection
            start_time = time.time()
            while not self.connected and (time.time() - start_time < 10):
                time.sleep(0.1)
                
            if not self.connected:
                logging.error("Initial connection timeout")
                
        except Exception as e:
            logging.error(f"Initial connection failed: {str(e)}")
            self._reconnect()

    def run(self):
        """Main loop to maintain connection and handle messages"""
        try:
            while True:
                if not self.connected:
                    self._reconnect()
                time.sleep(1)
        except KeyboardInterrupt:
            logging.info("Shutting down subscriber...")
            self.client.loop_stop()
            self.client.disconnect()
            logging.info("Cleanup completed")

if __name__ == "__main__":
    subscriber = MQTTSubscriber()
    subscriber.connect()
    subscriber.run()