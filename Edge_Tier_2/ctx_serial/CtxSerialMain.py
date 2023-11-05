import serial, json, sys, traceback
from edge_library.CtxEnums import COMMON
from edge_library.CtxUtils import prepare_generate_topic, get_serial_port
from edge_library.MQTTManager import MQTTManager


class CtxSerialModule:


    def __init__(self, badge_no: int=None, serial_port: int=0) -> None:
        self.data=None
        self.serial=None
        self.serial_port=serial_port
        self.module_badge_no=badge_no
        self.mqtt_manager=MQTTManager()
        self.line_len=45


    def open_serial(self):
        self.serial=serial.Serial(get_serial_port(self.serial_port), COMMON.DEFAULT_BAUD.value)
        
    def register_mqtt_and_start_communication(self) -> None:
        self.open_serial()

        self.write_serial_data(data=COMMON.SERIAL_INIT_MSG.value.replace("$", str(self.module_badge_no)))
        
        self.mqtt_manager.base_topic=prepare_generate_topic(self.module_badge_no, False)
        self.mqtt_manager.controller_topic=prepare_generate_topic(self.module_badge_no, True)
        self.mqtt_manager.serial=self.serial
        
        self.mqtt_manager.connect_client()
        self.mqtt_manager.start_loop() # Start listening sub-incoming stream


    def stop_all(self):
        self.mqtt_manager.stop_loop()


    def process_serial_data(self) -> None:
        # send upstream
        # self.publish
        try:
            if self.serial.in_waiting > 0:
                rawserial=self.serial.readline()
                cookedserial=rawserial.decode("utf-8").strip("\r\n")
                print(cookedserial)
                print("-"*self.line_len)
                print(f"Teensy says: {cookedserial}")
                if(cookedserial.lstrip().startswith('{') and cookedserial.rstrip().endswith('}')):
                    servedserial=json.dumps(cookedserial, indent=4, ensure_ascii=False)
                    self.mqtt_manager.publish(data=servedserial)
        except Exception as e:
            print(e)
            _typ, _val, _tb=sys.exc_info()
            traceback.print_exception(_typ, _val, _tb)
            print("There was some error parsing some data, waiting for next message...")
            print("-"*self.line_len)
        

    def write_serial_data(self, data=None) -> None:
        if data:
            self.data=data
        self.serial.write(str.encode(f"{data}\r\n"))


def start_serial_communication(metadata):
    all_ctxes=[]

    for k,v in metadata.items():
        all_ctxes.append(CtxSerialModule(badge_no=k,serial_port=v))
        
    try:
        for _ctx in all_ctxes:
            _ctx.register_mqtt_and_start_communication()

        while True:
            for _ctx in all_ctxes:
                _ctx.process_serial_data()

    except KeyboardInterrupt|Exception|serial.SerialException as e:
        if isinstance(e, KeyboardInterrupt):
            print("Exiting...\n")
        elif isinstance(e, serial.SerialException):
            print(e)
        else:
            print(e)
            print(e.__traceback__)
            _typ, _val, _tb=sys.exc_info()
            traceback.print_exception(_typ, _val, _tb)
        for _ctx in all_ctxes:
            _ctx.stop_all()
