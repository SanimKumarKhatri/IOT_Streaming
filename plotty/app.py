import eventlet
import threading

eventlet.monkey_patch()

from flask import Flask, render_template
from flask_socketio import SocketIO
import random
import time
import paho.mqtt.client as mqtt

latest_accel_data = {"time": "0.0.0", "ax": 0.0, "ay": 0.0, "az": 0.0}
latest_gyro_data = {"time": "0.0.0", "gx": 0.0, "gy": 0.0, "gz": 0.0}
latest_temp_data = {"time": "0.0.0", "temperature": 0.0}

accel_received  = False 
gyro_received = False
temp_received = False

def on_connect(client, userdata, flags, rc):
    print(f'Connected with code: {rc}')
    client.subscribe("mpu6050_test/accel_data")  
    client.subscribe("mpu6050_test/gyro_data")   
    client.subscribe("mpu6050_test/temp_data")  

def on_message(client, userdata, msg):
    global accel_received, gyro_received, temp_received
    payload = msg.payload.decode()
    print(f"Received: {msg.topic} -> {payload}")
    try:
        if msg.topic == "mpu6050_test/accel_data":
            ax, ay, az = map(float, payload.split(","))
            global latest_accel_data 
            latest_accel_data = {
                    "time": time.strftime("%H:%M:%S"),
                    "ax": ax,
                    "ay": ay,
                    "az": az
                }
            accel_received = True 

        elif msg.topic == "mpu6050_test/gyro_data":
            gx, gy, gz = map(float, payload.split(","))
            global latest_gyro_data
            latest_gyro_data = {
                    "time": time.strftime("%H:%M:%S"),
                    "gx": gx,
                    "gy": gy,
                    "gz": gz
            }
            gyro_received = True

        elif msg.topic == "mpu6050_test/temp_data":
            temp = float(payload)
            global latest_temp_data
            latest_temp_data = {
                    "time": time.strftime("%H:%M:%S"),
                    "temperature": temp
            }
            temp_received = True

    except ValueError:
        print(f"Error: Invalid data format from {msg.topic}")

app = Flask(__name__)
socketio = SocketIO(app, cors_allowed_origins="*")

@app.route("/")
def index():
    return render_template("index.html")

def send_data():
    global accel_received, gyro_received, temp_received

    while True:
        if accel_received == True:
            accel_received = False
            print("accel data")
            socketio.emit("update_accel", {
                "time": latest_accel_data["time"],
                "accel_x": latest_accel_data["ax"],
                "accel_y": latest_accel_data["ay"],
                "accel_z": latest_accel_data["az"]
            }) 
        
        if gyro_received == True:
            gyro_received = False
            print("gyro data")
            socketio.emit("update_gyro", {
                "time": latest_gyro_data["time"],
                "gyro_x": latest_gyro_data["gx"],
                "gyro_y": latest_gyro_data["gy"],
                "gyro_z": latest_gyro_data["gz"]
            }) 

        if temp_received == True:
            temp_received = False
            print("temperature data")
            socketio.emit("update_temp", {
                "time": latest_temp_data["time"],
                "temperature": latest_temp_data["temperature"]
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
