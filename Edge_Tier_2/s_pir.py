from ctx_serial import start_serial_communication
import sys, traceback
from argparse import ArgumentParser

_metadata={}
parser = ArgumentParser()
parser.add_argument(
    "-s", "--Sensor", nargs='*', help="""
    Accepts badge_no and rfcomm serial number in the format 
    [b_no, s_port] [b_no, s_port] [b_no, s_port] ....""",
    required=True)

args = parser.parse_args()

if args.Sensor:
    parsed=args.Sensor
    for v in parsed:
        if "," not in v:
            continue
        badge_no, serial_port = str(v).split(",")[:2]
        _metadata[badge_no] = serial_port
while True:
    try:
        ## Start the Serial PIR communication
        start_serial_communication(_metadata)
    except Exception as e:
        print(f"Caught exception: {e}")
        _typ, _val, _tb=sys.exc_info()
        traceback.print_exception(_typ, _val, _tb)
        print("Exiting....")
