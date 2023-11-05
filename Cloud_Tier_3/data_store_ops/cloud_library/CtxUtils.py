from .CtxEnums import MODULES, TOPIC_TYPE, SERIAL_PORTS, MQTT_ENUMS

def prepare_generate_topic(ctx_module: any=-1, is_controller: bool=False, is_esp:bool = False):
    mod=None
    if is_esp:
        mod=MODULES.ESP32.value
    else:
        mod=MODULES.__getitem__(f'CTx{ctx_module}').value
    return f"{MQTT_ENUMS.TOPIC_PREFIX.value}/{mod}/{TOPIC_TYPE.SEND.value if is_controller else TOPIC_TYPE.RECEIVE.value}"

def get_serial_port(serial: any):
    return f"/dev/{SERIAL_PORTS.__getitem__(f'RFCOMM{serial}').value}"
