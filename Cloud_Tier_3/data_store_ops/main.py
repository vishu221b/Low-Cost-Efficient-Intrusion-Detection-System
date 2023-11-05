from cloud_library import MQTTManager, CtxEnums, CtxUtils
from db import get_collection
import ast
from datetime import datetime
from base64 import b64encode


deleted=False

def upload_data_to_mongo(payload, topic):
    global deleted
    _default_coll=get_collection()
    _coll=get_collection(topic)
    if not deleted:
        _coll.delete_many({})
        deleted=True
    _str_pl=payload
    print(_str_pl)
    _pl=None
    if "\\x" in str(_str_pl):
        _pl={'raw_image': b64encode(_str_pl).decode()}
    else:
        print(_pl)
        _pl=ast.literal_eval(_str_pl.decode())

    if not str(_pl).startswith("{") or not isinstance(_pl, dict):
        try:
            _pl=ast.literal_eval(_pl)
        except Exception as e:
            print(e)
            _pl={'event_or_command': _pl}
    print(_pl)
    print(type(_pl))
    _pl['datetime']=datetime.now()
    _pl['topic']=topic
    res=_coll.insert_one(_pl)
    res2=_default_coll.insert_one(_pl)
    print(res.inserted_id)
    print('-'*40)
    print(res2.inserted_id)

managers=[]

stream_done=False
def _run_main():
    try:
        for module in CtxEnums.MODULES.__members__:
            manager=MQTTManager.MQTTManager(payload_callback=upload_data_to_mongo, subscribe_base=True)
            manager.connect_client(host=CtxEnums.MQTT_ENUMS.AWS_HOST.value)
            if module != CtxEnums.MODULES.ESP32.name:
                manager.base_topic=CtxUtils.prepare_generate_topic(module[-1],False, False)
                manager.controller_topic=CtxUtils.prepare_generate_topic(module[-1],True, False)
                managers.append(manager)
            else:
                if not stream_done:
                    manager.base_topic=f"{CtxUtils.prepare_generate_topic(-1, False, True)}/{CtxEnums.ESP32_CONFIG.TOPIC_IMAGE_STREAM_SUFFIX.value}"
                    managers.append(manager)
                else:
                    manager.base_topic=f"{CtxUtils.prepare_generate_topic(-1, False, True)}/{CtxEnums.ESP32_CONFIG.TOPIC_DATA_STREAM_SUFFIX.value}"
                    managers.append(manager)
            manager.start_loop()
    except Exception as e:
        if isinstance(e, KeyboardInterrupt):
            print("Exiting....\n")
            [x.stop_loop() for x in managers]
            exit(1)
        else:
            print(e)
            print("Retrying...")

if __name__ == "__main__":
    _run_main()
    c=0
    while True:
       c+=1
       if c==0x1123:
           c=0 
