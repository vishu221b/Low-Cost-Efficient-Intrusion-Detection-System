from ESP32_Websockets import start_esp32_communication
import sys, traceback

while True:
    try:
        start_esp32_communication()
    except Exception as e:
        print(f"Caught exception: {e}")
        _typ, _val, _tb=sys.exc_info()
        traceback.print_exception(_typ, _val, _tb)
        print("Exiting....")

