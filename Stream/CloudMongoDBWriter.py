from pymongo import MongoClient
from pymongo import errors
import os
import time
from dotenv import load_dotenv
load_dotenv()
class CloudMongoDB:
    def __init__(self):
        self.mongoClient=None
        self.Sensor_db=None
        self.Sensor_collection_IMU=None
        self.username=os.getenv('MONGO_USERNAME')
        self.password = os.getenv('MONGO_PASSWORD')
    def mongoConnect(self):
        URI=f"mongodb+srv://{self.username}:{self.password}@iotprojectcluster.io48u.mongodb.net/"
        print(URI)
        try:
            self.mongoClient=MongoClient(URI, serverSelectionTimeoutMS=5000)
            self.Sensor_db=self.mongoClient['Sensor']
            self.Sensor_collection_IMU=self.Sensor_db["IMU"]
            print("Successfully connected to cloud db cluster.")
        except errors.ConfigurationError:
            print("MongoDB Out of Reach")

        except errors.OperationFailure:
            print("Authentication failed") 
    def isMongoDbConnected(self):
        '''
        Check connection
        '''
        try:
            self.mongoClient.server_info()
            return True
        except:
            return False
    def insertDocument(self,document):
        '''
        Insert into mongoDB
        '''
        try:
            self.Sensor_collection_IMU.insert_one(document)
        except errors._OperationCancelled:
            print("Connection Error")
    def load_documents(self):
        pass