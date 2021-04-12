Bmqtt_port = 1883thon3
#!/usr/bin/env python3
import paho.mqtt.client as mqtt
import requests
import StringIO
import time
import os
from datetime import datetime
from influxdb import InfluxDBClient
#====================================================
# set influxdb configuration -------------------------
dbhost = "10.0.6.142"
dbport = 8086
dbuser = "admin"
dbpassword = "12345678"
dbname = "iot"

#====================================================
# set mqtt configuration ===========================
mqtt_server = "10.0.6.218"
mqtt_port = 1883
mqtt_user = "user1"
mqtt_password = "123456"
# =================================================

def saveData(topic, content):
    now = datetime.now()
    date = now.strftime("%Y-%m-%d")
    time = now.strftime("%H:%M:%S")
    filename = 'data/dht11-log-'+ date + '.txt'

    result = os.path.exists(filename)
    if result == False:
        f = open(filename, 'wb')
    if result == True:
        f = open(filename, 'ab')
    data = time + "," + topic + "=" +content + "\r"

    f.write(data)

#====================================================

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
# set client subscriber ----------------------
    client.subscribe("temperature")                                                                                                                                                                                  client.subscribe("humidity")

#    client.subscribe("demo")
#====================================================

def on_message(client, userdata, msg):
    print("Received a message on topic: " + msg.topic)
    # Use utc as timestamp
    now = datetime.now()
    receiveTime = now.strftime("%Y-%m-%d %H:%M:%S")
    #receiveTime=datetime.datetime.utcnow()
    message=msg.payload.decode("utf-8")
    isfloatValue=False

    try:
        # Convert the string to a float so that it is stored as a number and not a string in the database
        val = float(message)
        isfloatValue=True
    except:
        print("Could not convert " + message + " to a float value")
        isfloatValue=False
 try:
        # Convert the string to a float so that it is stored as a number and not a string in the database
        val = float(message)
        isfloatValue=True
    except:
        print("Could not convert " + message + " to a float value")
        isfloatValue=False

    if isfloatValue:                                                                                                                                                                                                     print(str(receiveTime) + ": " + msg.topic + " " + str(val))

        if (msg.topic == "temperature"):
            json_data = [
                {
                    "measurement": msg.topic,
                    "tags": {
                        "host": "iotlab",
                        "sensor": "sensor1"
                    },
                    "fields": {
                        "temperature": val
                    }
                }
            ]

        if (msg.topic == "humidity"):
            json_data = [
                {
                    "measurement": msg.topic,
                    "tags": {
                        "host": "iotlab",                                                                                                                                                                                                "sensor": "sensor1"
                    },
                    "fields": {
                        "humidity": val
                    }
                }
            ]

        saveData(msg.topic, message)
        print("Finished writing to File !!!")
        dbclient.create_database(dbname)
        dbclient.write_points(json_data)
        print("Finished writing to InfluxDB")
        print("Waiting data from client...")
#   client.publish("demo")
#====================================================
# Set up a client for InfluxDB
dbclient = InfluxDBClient(dbhost, dbport, dbuser, dbpassword, dbname)  

#====================================================
# Initialize the MQTT client that should connect to the Mosquitto broker
client = mqtt.Client()
client.on_connect = on_connect                                                                                                                                                                                   client.on_message = on_message
connOK=False

while(connOK == False):
    try:
        client.username_pw_set(mqtt_user, mqtt_password)
        client.connect(mqtt_server, mqtt_port, 60)
        connOK = True
    except:
        connOK = False
    time.sleep(2)
#====================================================
# Blocking loop to the Mosquitto broker
client.loop_forever()
