import eventlet
eventlet.monkey_patch()

import threading
from flask import Flask, render_template
from flask_socketio import SocketIO
import time
import paho.mqtt.client as mqtt

latest_data = {"time": "0.0.0", "ax": 0.0, "ay": 0.0, "az": 0.0, "gx": 0.0, "gy": 0.0, "gz": 0.0, "temperature": 0.0}

data_received = False

def on_connect(client, userdata, flags, rc):
    print(f'Connected with code: {rc}')
    client.subscribe("mpu6050_iot/all_data")  

def on_message(client, userdata, msg):
    global data_received,accel_received, gyro_received, temp_received
    payload = msg.payload.decode()
    print(f"Received: {msg.topic} -> {payload}")
    try:
        if msg.topic == "mpu6050_iot/all_data":
            ax, ay, az, gx, gy, gz, temp = map(float, payload.split(","))
            global latest_data 
            latest_data = {
                    "time": time.strftime("%H:%M:%S"),
                    "ax": ax,
                    "ay": ay,
                    "az": az,
                    "gx": gx,
                    "gy": gy,
                    "gz": gz,
                    "temperature": temp
                }
            data_received = True 

    except ValueError:
        print(f"Error: Invalid data format from {msg.topic}")

app = Flask(__name__)
socketio = SocketIO(app, cors_allowed_origins="*")

@app.route("/")
def index():
    return render_template("index.html")

def send_data():
    global data_received, accel_received, gyro_received, temp_received

    while True:

        if data_received == True:
            data_received = False
            print("all data")
            socketio.emit("update_data", {
                "time": latest_data["time"],
                "accel_x": latest_data["ax"],
                "accel_y": latest_data["ay"],
                "accel_z": latest_data["az"],
                "gyro_x": latest_data["gx"],
                "gyro_y": latest_data["gy"],
                "gyro_z": latest_data["gz"],
                "temperature": latest_data["temperature"]
            }) 
            
        eventlet.sleep(1)  

@socketio.on("connect")
def handle_connect():
    print("Client connected!")

def mqtt_loop():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message 
    client.connect("mqtt.eclipseprojects.io", 1883)
    client.loop_forever()

if __name__ == "__main__":
    mqtt_thread = threading.Thread(target=mqtt_loop)
    mqtt_thread.start()
    socketio.start_background_task(send_data)  
    socketio.run(app, debug=True, host="0.0.0.0", port=5000)
