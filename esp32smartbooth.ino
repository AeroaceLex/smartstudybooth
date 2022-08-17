/* ESP32 AWS IoT
 *  
 * Simplest possible example (that I could come up with) of using an ESP32 with AWS IoT.
 *  
 * Author: Anthony Elder 
 * License: Apache License v2
 */
#include <WiFiClientSecure.h>
#include <PubSubClient.h> // install with Library Manager, I used v2.6.0
#include <ESP32_Servo.h>
#define TRIG_PIN 23 // ESP32 pin GIOP23 connected to Ultrasonic Sensor's TRIG pin
#define ECHO_PIN 22 // ESP32 pin GIOP22 connected to Ultrasonic Sensor's ECHO pin

Servo myservo;
int pos = 0;
int servopin = 13;
float duration_us, distance_cm;
const char* ssid = "";
const char* password = "";

const char* awsEndpoint = "a2o33nvjt80dyp-ats.iot.ap-southeast-1.amazonaws.com";

// Update the two certificate strings below. Paste in the text of your AWS 
// device certificate and private key. Add a quote character at the start
// of each line and a backslash, n, quote, space, backslash at the end 
// of each line:

// xxxxxxxxxx-certificate.pem.crt
const char* certificate_pem_crt = \

"-----BEGIN CERTIFICATE-----\n" \
R"EOF(MIIDWjCCAkKgAwIBAgIVAIVsnMcyFSsKlhdntgFX8Y3lBzM7MA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMjA4MDkwODAw
MDJaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCxCci5fT8rbDrZHpD5
N93mfhyCQYiiBqGI7DkW3VDNcvAe/HkaPDcZN/zPe4JVztxVr5kvnjJ2al4JPD72
ZB8kVNWle4TXFywVIwOCp+A0mbctyq5Vy8yZKFqFC6tyRGOnv+1nILxWiPVmCsoR
3LI4Gbut8G270TWoNMGpJY8J2Ps2lxjkwUvqaZKh+ym3rD1Ua0BCwiOVlSDCA7hc
loTo6YnUHQ6jSv1MptB21VHpleYM2a9Q7s/57wvO55/R9IuYrSLHtHsjSUqERqwS
Pe14PPuLJTGKebu7WE5noQKTHNm9Ey+DR197c+XT9FrI68qqMkz4EVMURwpl/gbp
4tRpAgMBAAGjYDBeMB8GA1UdIwQYMBaAFDK/kFCDu+2DhoEA+RKaUc3rvQR/MB0G
A1UdDgQWBBTTKaxZxCzv+JDzlReLm4hXyyyitzAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAiwyJIAZcQmPmuEDoFHEYsY97
D6IpAioOapZ0QoBPL0BGM4DEznR77YLnuAUUx8wr7NpBAw4misZwxjWof1z6h+3l
Rob2dkfAH7nsmn0ZomBBUzcdZf2u8Ur07zfHncHlocZ+u4UBjydGmS5fctcE8xdF
tV5HkQ+TeNm2MHup+ayMRhbAs47Twl+0BB9oVlh+EvjUxz6f73d6Piuz7YzxKycq
WESdkfPXAkNSWMvfEgtYtPEhmwXAkNWM1rEn1MHWBa2q5/UXhwhzxrwFyncA9A6h
dmvl4xhUCrSlmrE1aaxHou2GLwZ+c2NvRjwoPMFgDwdh6sPbujCE6Cmj53bqfg==)EOF"
"-----END CERTIFICATE-----\n";

// xxxxxxxxxx-private.pem.key
const char* private_pem_key = \

"-----BEGIN RSA PRIVATE KEY-----\n" \
R"EOF(MIIEowIBAAKCAQEAsQnIuX0/K2w62R6Q+Tfd5n4cgkGIogahiOw5Ft1QzXLwHvx5
Gjw3GTf8z3uCVc7cVa+ZL54ydmpeCTw+9mQfJFTVpXuE1xcsFSMDgqfgNJm3Lcqu
VcvMmShahQurckRjp7/tZyC8Voj1ZgrKEdyyOBm7rfBtu9E1qDTBqSWPCdj7NpcY
5MFL6mmSofspt6w9VGtAQsIjlZUgwgO4XJaE6OmJ1B0Oo0r9TKbQdtVR6ZXmDNmv
UO7P+e8Lzuef0fSLmK0ix7R7I0lKhEasEj3teDz7iyUxinm7u1hOZ6ECkxzZvRMv
g0dfe3Pl0/RayOvKqjJM+BFTFEcKZf4G6eLUaQIDAQABAoIBAAnaEIh59sz0zeWN
NOYZfMi51d5NxUu28MBM3PRvxnJYZhJjXF3gYfSOUAw/xEYAJ/TAu9aRiB9c1fTP
nHVbpvv/mJAH6qGJkZNvil939hsXPsFWmRL/L1iKjVcACj9mnOEV5iEqCjEKmBVg
nlQWIvjKPc51JbYg80pz/R7rjyUuDSy3Zsxvn9Yu7dVm+r2It3ryLd3LOZiyit/R
VTcgBQt47AAHzeVMFBfakbeLIWjKc0FufBLSRJypi1p0F6WDUJwGXuyh7H/wI6Pq
4mTxHCLhfjH/3ZeJ/XBk4rHdPCZhrf+pdUAfx/lhRgTGxOBM8i/B8uoZ+MgSUxAe
2CbfbH0CgYEA5/ioirFwbNgDftYd5JpuzqgbeZFxgEkCj4feOwGZvILi7QXXv6MZ
Kohr+Wknc4QsV5Or3KGaaKi5KTlQXe3zlYuTX82LH9AGTlW5jd9+A1vJrfU4h3o3
KRqFjHjZ58owpaMG+SOn+ZdhEtCWNX6zT3mwEsQOSsB2dyW3Vd040TsCgYEAw2Bt
XHGefNAUymtuElPRwK8ksJVNB9LGZqnzuCV7it+a+I20koVbEgRFZj4fzEqbmiJL
1LL0sqQZdITsZYHN2c8ev799TN/1B0TEIPKyXIdz/+pvQU39kqvkKNsO17Sl+24/
z8c6T47FBUTf0dLU+yzZXKpMQPix0gtJIRmcFqsCgYEAt1WbPyJFzIBdIt1u+sHj
WYkbTdVZcx5A29dyGAiHzKlVWCcKAPeSaD8xtwkYmP+ZXA3mApXmR1kyHhaiIdpy
bXrGmy0EC7yJf1fZMvuzyEu+9bGdX9WDywSfEApjjgIKuJ5EYJuBoo21tv0L+nOM
m/983Rf+g63g8wHoCACzawkCgYAl0nH5Vml3AhU0Rm3NkHpnkcBJiu2ZmR0dXMM4
n9D1xmnKuiv9vWEu8LK6o0Ymlz7eLMt/RpGGT6w52U2mLOW7x5PNTPWlvylBGffZ
5QHQVcyH+0tEWiO/lBUcL6QE8Ay9ys8BmzDXm+zaI0Ys6WKfqUP+41zVqw9JeTPT
+5Ay9wKBgHekG/dN7GAK3ymmhYtlTcuOGR1BaYsRWDdTWUY1g/Eq/8T9e5yTiOGa
UFVwp0SqRxdHktov2lPm5nn+Tv/Oby7uRqu2j0HY4TlWrt4kLEs+E9eHKqUFrcyR
sHC/i3M5JmN2+aIbEN2nQUocKcas6afhUp56C/uke+5IHkDUYADH)EOF"
"-----END RSA PRIVATE KEY-----\n";

/* root CA can be downloaded in:
  https://www.symantec.com/content/en/us/enterprise/verisign/roots/VeriSign-Class%203-Public-Primary-Certification-Authority-G5.pem
*/
const char* rootCA = \
"-----BEGIN CERTIFICATE-----\n" \
R"EOF(MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5)EOF"
"-----END CERTIFICATE-----\n";

WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 
void setup() {
  myservo.attach(servopin);
  myservo.write(pos);
  pinMode(18, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT); 
  Serial.begin(115200); delay(50); Serial.println();
  Serial.println("ESP32 AWS IoT Example");
  Serial.printf("SDK version: %s\n", ESP.getSdkVersion());

  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: "); Serial.println(WiFi.localIP());

  wiFiClient.setCACert(rootCA);
  wiFiClient.setCertificate(certificate_pem_crt);
  wiFiClient.setPrivateKey(private_pem_key);
}

unsigned long lastPublish;
int msgCount;

void loop() {
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration_us = pulseIn(ECHO_PIN, HIGH);
  distance_cm = 0.017 * duration_us;
  Serial.print("distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");
  delay(500);
  pubSubCheckConnect();
  if(distance_cm < 10){
    myservo.write(0);
    digitalWrite(27,HIGH);
    delay(500);
    digitalWrite(27,LOW);
  }
  
}
void msgReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  digitalWrite(18,HIGH);
  delay(500);
  digitalWrite(18,LOW);
    for (pos = 0; pos <= 270; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos); // tell servo to go to position in variable 'pos'
    }
}

void pubSubCheckConnect() {
  if ( ! pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); Serial.print(awsEndpoint);
    while ( ! pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect("ESPthing");
      delay(1000);
    }
    Serial.println(" connected");
    pubSubClient.subscribe("inTopic");
  }
  pubSubClient.loop();
}
