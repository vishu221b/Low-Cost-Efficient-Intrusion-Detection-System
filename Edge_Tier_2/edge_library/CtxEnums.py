from enum import Enum

class MODULES(Enum):
    CTx1 = "CTx1"
    CTx2 = "CTx2"
    CTx3 = "CTx3"
    ESP32= "ESP32"

class ESP32_CONFIG(Enum):
    TOPIC_IMAGE_STREAM_SUFFIX="STREAM"
    TOPIC_DATA_STREAM_SUFFIX="DATA"

class TOPIC_TYPE(Enum):
    SEND="SEND"
    RECEIVE="RECEIVE"

class MQTT_ENUMS(Enum):
    AWS_HOST="0.0.0.0"
    AWS_PORT="1883"
    TOPIC_PREFIX="LCE_IDS"

class SERIAL_PORTS(Enum):
    RFCOMM0="rfcomm0"
    RFCOMM1="rfcomm1"
    RFCOMM2="rfcomm2"

class COMMON(Enum):
    DEFAULT_BAUD=9600
    SERIAL_INIT_MSG="EDGE_CTx$ says, Beginning Serial communication!!"

class WEBSOCK_CONFIG(Enum):
    SERVER="0.0.0.0"
    PORT="8888"
