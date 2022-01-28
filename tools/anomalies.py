import sys
import numpy as np
import correlation
import matplotlib.pyplot as plt

def filter_fields(obj):
    return {
        'timestamp': obj["timestamp"],
        'altitude': obj["altitude"],
        'anomaly_score': obj["anomaly_score"],
        'bmp_temperature': obj["bmp_temperature"],
        'humidity': obj["humidity"],
        'igrometer_percent': obj["igrometer_percent"],
        'ldr_percent': obj["ldr_percent"],
        'mq135_percent': obj["mq135_percent"],
        'pressure': obj["pressure"],
        'rain_meter_percent': obj["rain_meter_percent"],
        'temperature': obj.get("temperature", 0),
    }

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"USAGE:", file=sys.stderr)
        print(f"  {sys.argv[0]} <data.csv>", file=sys.stderr)
        exit(1)
    input_path = sys.argv[1]
    data = correlation.get_data(input_path, filter_fields)

    anomaly = list(filter(lambda e: e["anomaly_score"] >= 0,[ dict(zip(data,t)) for t in zip(*data.values())]))
    anomaly = {k: [dic[k] for dic in anomaly] for k in anomaly[0]}
    total_len = len(data["temperature"])
    anomaly_len = len(anomaly["temperature"])

    timestamp = anomaly.pop("timestamp", None)

    day_anomaly = list(filter(lambda e: e["ldr_percent"] >= 50.0,[ dict(zip(anomaly,t)) for t in zip(*anomaly.values())]))
    day_anomaly = {k: [dic[k] for dic in day_anomaly] for k in day_anomaly[0]}
    day_len = len(day_anomaly["temperature"])
    
    print(f"")
    print(f"Anomalies: {anomaly_len}({(anomaly_len*100)/total_len}%)")
    print(f"Anomalies day: {day_len}({(day_len*100)/anomaly_len}%) night: {anomaly_len-day_len}({((anomaly_len-day_len)*100)/anomaly_len}%)")
    print(f"")

    print(f"")
    print(f"Anomaly statistics:")
    for k,v in anomaly.items():
        print(f"  {k}:")
        print(f"    mean: {np.mean(v)}, std: {np.std(v)}, min: {np.min(v)}, max: {np.max(v)}")
    print(f"")

