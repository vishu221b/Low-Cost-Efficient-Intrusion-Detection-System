import asyncio
from websockets.server import serve
from edge_library.MQTTManager import MQTTManager
from edge_library.CtxEnums import WEBSOCK_CONFIG, ESP32_CONFIG
from edge_library.CtxUtils import prepare_generate_topic
from threading import Thread

class ESP32:

    def __init__(self) -> None:
        self.mqtt_manager=MQTTManager()
        self.image_stream_topic=f"{prepare_generate_topic(0, False, True)}/{ESP32_CONFIG.TOPIC_IMAGE_STREAM_SUFFIX.value}"
        self.data_stream_topic=f"{prepare_generate_topic(0, False, True)}/{ESP32_CONFIG.TOPIC_DATA_STREAM_SUFFIX.value}"

    def start_websocket_server(self) -> None:
        self.mqtt_manager.connect_client()
        # t=Thread(target=asyncio.run_coroutine_threadsafe, args=(self.create_websocket_server()))
        # t.start()
        # t.join()
        

    async def create_websocket_server(self) -> None:
        print("STARTING: Websockets Server")
        asyncio.sleep(0)
        # _task=_e_loop.create_task(self.create_websocket_server())
        # _e_loop.run_until_complete(serve(self.process_data, WEBSOCK_CONFIG.SERVER.value, WEBSOCK_CONFIG.PORT.value, _e_loop))
        async with serve(self.process_data, WEBSOCK_CONFIG.SERVER.value, WEBSOCK_CONFIG.PORT.value):
            await asyncio.Future()

    async def process_data(self, ws_data) -> None:
        print(f"Incoming: {ws_data}")
        while True:
            incoming=await ws_data.recv()
            await self.perform_type_specific_ops(data=incoming)

    async def perform_type_specific_ops(self, data) -> None:
        if type(data) == bytes:
            # This is an image, publish it as it is or after base64 conversion?->Later->Cloud-Tier
            self.mqtt_manager.publish(data=data, target_topic=self.image_stream_topic)
            # print(f"IMAGE: {data}")
        elif type(data) == str:
            # Probably some commmand or Flash status? Publish it!
            self.mqtt_manager.publish(data=data, target_topic=self.data_stream_topic)
            # print(f"DATA: {data}")
        # No idea what this gonna process?
        # else:
        #     pass

def start_esp32_communication() -> None:
    exx=ESP32()
    exx.start_websocket_server()
    _e_loop=asyncio.new_event_loop()
    asyncio.run(exx.create_websocket_server())

