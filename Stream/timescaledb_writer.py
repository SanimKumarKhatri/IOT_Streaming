import psycopg2
from datetime import datetime
import re
import logging
from urllib.parse import urlparse
from subscribe_esp32_data import MQTTSubscriber
import time


class TimescaleSubscriber(MQTTSubscriber):
    def __init__(self):
        # Initialize parent MQTTSubscriber
        super().__init__()
        
        # Set up TimescaleDB connection
        self._setup_database()

    def _setup_database(self):
        """Initialize database connection and tables"""
        try:
            # credentials = parse_timescale_credentials(creds_file)
            # self.conn = psycopg2.connect(
            #     host=credentials['host'],
            #     port=credentials['port'],
            #     database=credentials['database'],
            #     user=credentials['user'],
            #     password=credentials['password'],
            #     sslmode=credentials['sslmode']
            # )
       
            self.conn = psycopg2.connect(
                host='localhost',
                port='5432',
                user='postgres',
                database='sensor',
                password='1234'
            )  
            self.cursor = self.conn.cursor()
                          
            # Check connection
            self.cursor.execute("SELECT 1")
            logging.info("Database connection verified")

            # Create table and hypertable
            # self.cursor.execute('''
            #     DROP TABLE IF EXISTS sensor_data
            # ''')
            self.cursor.execute('''
                CREATE TABLE IF NOT EXISTS sensor_data (
                    time TIMESTAMPTZ NOT NULL,
                    topic TEXT NOT NULL,
                    ax  NUMERIC(5,2) NOT NULL, 
                    ay  NUMERIC(5,2) NOT NULL,
                    az  NUMERIC(5,2) NOT NULL, 
                    gx  NUMERIC(5,2) NOT NULL, 
                    gy  NUMERIC(5,2) NOT NULL, 
                    gz  NUMERIC(5,2) NOT NULL, 
                    temp NUMERIC(5,2) NOT NULL
                );''')
            self.cursor.execute('''
                SELECT create_hypertable('sensor_data',by_range('time'), 
                       if_not_exists => TRUE);''')
            self.conn.commit()
            logging.info("Database setup completed")
            
        except Exception as e:
            logging.error(f"Database initialization failed: {e}")
            raise

    def _on_message(self, client, userdata, message):
        """Override message handler to add database storage"""
        # First calling parent message handling
        super()._on_message(client, userdata, message)

        # Then handling database insertion
        try:

            with self.conn:
                with self.conn.cursor() as cursor:
                    payload = message.payload.decode()
                    parts = payload.split(',')
                    timestamp = parts[0]
                    ax, ay, az, gx, gy, gz, temp = map(float, parts[1:])
                    cursor.execute('''
                        INSERT INTO sensor_data (time, topic, ax, ay, az, gx, gy, gz, temp)
                        VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s)
                    ''', (timestamp, message.topic, ax, ay, az, gx, gy, gz, temp))

                self.conn.commit()
                print("Message Written into the DB")
        except Exception as e:
            logging.error(f"Database error: {e}")
            self.conn.rollback()

    def cleanup(self):
        """Proper resource cleanup"""
        self.client.loop_stop()
        self.client.disconnect()
        if hasattr(self, 'conn'):
            self.cursor.close()
            self.conn.close()
        logging.info("All connections closed")

if __name__ == "__main__":

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