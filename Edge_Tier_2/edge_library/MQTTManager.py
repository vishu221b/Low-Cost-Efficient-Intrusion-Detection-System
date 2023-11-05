from .CtxEnums import MQTT_ENUMS

import paho.mqtt.publish as mqtt_publish
import paho.mqtt.client as mqtt_client


class MQTTManager:

    def __init__(self, serial=None, payload_callback=None) -> None:
        self.serial=serial
        self.base_topic=None
        self.controller_topic=None
        self.payload_callback=payload_callback
        self.client=mqtt_client.Client()


    def connect_client(self, host=None):
        self.client.on_connect=self.on_connect
        self.client.on_message=self.on_message
        self.client.connect(host or MQTT_ENUMS.AWS_HOST.value)


    def publish(self, data=None, target_topic=None) -> None:
        mqtt_publish.single(
            topic=target_topic or self.base_topic, 
            payload=data,
            hostname=MQTT_ENUMS.AWS_HOST.value
        )


    def start_loop(self) -> None:
        self.client.loop_start()
    

    def stop_loop(self) -> None:
        self.client.loop_stop()
    

    def on_connect(self, client, userdata, flags, rc):
        print(f"Established connection to MQTT, subscribing to {self.controller_topic}!!")
        if self.controller_topic:
            client.subscribe(self.controller_topic)

    def on_message(self, client, userdata, msg):
        print('-'*100)
        print(client)
        print(msg.topic, " :: ", msg.payload)
        print(userdata)
        print('-'*100)
        if self.serial:
            self.serial.write(str.encode(str(msg.payload)[2:-1]))
        elif self.payload_callback:
            self.payload_callback(msg.payload, msg.topic)
