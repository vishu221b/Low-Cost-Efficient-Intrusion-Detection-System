def parse_topic_for_collection_name(topic_name):
    s_up=topic_name.split("/")
    print(f"SPLIT: {s_up}")
    if "ESP32" in s_up:
        return '_'.join(s_up[1::2])
    return s_up[-2]
