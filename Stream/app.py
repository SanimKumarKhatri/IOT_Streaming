import eventlet
eventlet.monkey_patch()
from flask import Flask, render_template
import psycopg2
import logging
from timescaledb_writer import TimescaleSubscriber
import threading
from flask_socketio import SocketIO
from dotenv import load_dotenv
import os
load_dotenv()
app = Flask(__name__)
socketio = SocketIO(app, cors_allowed_origins="*")
# Database Connection
def get_db_connection():
    try:
        conn = psycopg2.connect(
            host='localhost',
            port=os.getenv('MONGODB_PORT'),
            user=os.getenv('MONGO_USERNAME'),
            database=os.getenv('MONGODB'),
            password=os.getenv('MONGO_PASSWORD')
        )
        return conn
    except Exception as e:
        logging.error(f"Error connecting to database: {e}")
        return None
# Fetch Data from TimescaleDB
def fetch_data():
    while True:
        try:
            conn= get_db_connection()
        except:
            raise
        try:
            cursor = conn.cursor()
            cursor.execute("SELECT * FROM sensor_data ORDER BY time DESC limit 1")
            rows = cursor.fetchall()
            cursor.close()
            conn.close()
            # Convert data to JSON format
            data = []
            for row in rows:
                data.append({
                'time': row[0].strftime('%Y-%m-%d %H:%M:%S'),
                'ax': float(row[2]),
                'ay': float(row[3]),
                'az': float(row[4]),
                'gx': float(row[5]),
                'gy': float(row[6]),
                'gz': float(row[7]),
                'temp':float(row[8])
                })
            socketio.emit("new_sensor_data",{
                "sensor_data":data
            })
            print(f"Emmitted: {data}")
            eventlet.sleep(0.9)
        except Exception as e:
            logging.error(f"Error fetching data: {e}")
# Run MQTT Subscriber AND write data to DB in Background
def start_mqtt_subscriber_and_write_db():
    """Runs MQTT subscriber in the background."""
    global subscriber
    try:
        subscriber = TimescaleSubscriber()
        subscriber.connect()
            
        if subscriber.connected:
            logging.info("Successfully connected to both MQTT broker and TimescaleDB!")
            subscriber.run()
        else:
            logging.error("Failed to establish MQTT connection")
            
    except KeyboardInterrupt:
        logging.info("Received keyboard interrupt, shutting down...")
    except Exception as e:
        logging.error(f"Critical error occurred: {e}")
    finally:
        subscriber.cleanup()
@app.route("/")
def index():
    return render_template("index.html")
# Run Flask App
if __name__ == '__main__':
    mqtt_sub_db_write_thread = threading.Thread(target=start_mqtt_subscriber_and_write_db)
    mqtt_sub_db_write_thread.start() 
    socketio.start_background_task(fetch_data)
    app.run(debug=True, host='0.0.0.0', port=5000)
