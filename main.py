from flask import Flask, render_template, request, redirect, url_for 
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import sys
from http.server import BaseHTTPRequestHandler, HTTPServer


app = Flask(__name__)

@app.route("/")
def login():
    return render_template("index.html")

@app.route("/loggedin", methods=["POST", "GET"])
def home():
    if request.method == "POST":

        myMQTTClient = AWSIoTMQTTClient("test")

        myMQTTClient.configureEndpoint("a2o33nvjt80dyp-ats.iot.ap-southeast-1.amazonaws.com", 8883)
        myMQTTClient.configureCredentials("certificates/x590.pem","certificates/private.key", "certificates/deviceCert.crt")

        myMQTTClient.connect()
        print("Client Connected")

        msg = "Sample data from the device"
        topic = "inTopic"
        myMQTTClient.publish(topic, msg, 0)  
        print("Message Sent")
        return render_template("loggedin.html")
    else:
        return render_template("loggedin.html")

   
@app.route("/loggedout")
def logout():
        return render_template("loggedout.html")
    
if __name__ == "__main__":
    app.run(debug=True)