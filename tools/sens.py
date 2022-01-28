import sys
from itertools import groupby
from dateutil import parser
import time
import json
import time, hmac, hashlib
import requests
import csv
import random

# Your API & HMAC keys can be found here (go to your project > Dashboard > Keys to find this)
HMAC_KEY = "8466a73127129c4a7dfaeadc85003d6f"
API_KEY = "ei_e9971b372e26a8a3e4c1ed3126ad6446843ab0933ee1d280cfe85686208a2bd2"

FILTER_DATE = parser.parse("2021-12-23T00:00:00.0Z")

def filter_date(obj):
    return obj["timestamp"] > FILTER_DATE

def filter_fields(obj):
    return {
        'timestamp': obj["timestamp"],
        'altitude': obj["altitude"],
        'bmp_temperature': obj["bmp_temperature"],
        'humidity': obj["humidity"],
        'igrometer_percent': obj["igrometer_percent"],
        'ldr_percent': obj["ldr_percent"],
        'mq135_percent': obj["mq135_percent"],
        'pressure': obj["pressure"],
        'rain_meter_percent': obj["rain_meter_percent"],
        'temperature': obj.get("temperature", 0),
    }

def merge(d):
    r = {}
    r["timestamp"] = parser.parse(d[0][0]) #time.mktime(parser.parse(d[0][0]).timetuple())
    #r["measurement"] = d[0][3]
    #r["host"] = d[0][4]
    #r["location"] = d[0][5]
    for k in d:
        r[k[2]] = k[1]
    return r

def create_data(input_path, fil):
    data = []
    with open(input_path, "r") as f:
        data = list(map(fil,list(filter(filter_date,[merge(list(group[1])) for group in 
            groupby(sorted([line.split(",")[5:] for line in 
                (line_raw.rstrip("\n") for line_raw in 
                    f.readlines()[4:][:-2])], key=lambda x: x[0]), key=lambda x: x[0])]))))
    return data

def to_list(obj):
    return [
            float(obj["altitude"]),
            float(obj["bmp_temperature"]),
            float(obj["humidity"]),
            float(obj["igrometer_percent"]),
            float(obj["ldr_percent"]),
            float(obj["mq135_percent"]),
            float(obj["pressure"]),
            float(obj["rain_meter_percent"]),
            float(obj["temperature"]),
    ]

def upload_data(data_line, dataset):
    # empty signature (all zeros). HS256 gives 32 byte signature, and we encode in hex, so we need 64 characters here
    emptySignature = ''.join(['0'] * 64)
    # here we have new data every 16 ms
    INTERVAL_MS = 900

    if INTERVAL_MS <= 0:
        raise Exception("Interval in miliseconds cannot be equal or lower than 0.")

    # here we'll collect 2 seconds of data at a frequency defined by interval_ms
    values_list = [to_list(data_line)]

    data = {
        "protected": {
            "ver": "v1",
            "alg": "HS256",
            "iat": time.time() # epoch time, seconds since 1970
        },
        "signature": emptySignature,
        "payload": {
            "device_type": "ESP8266",
            "interval_ms": INTERVAL_MS,
            "sensors": [
                { 'name': 'altitude', 'units': 'm'},
                { 'name': 'bmp_temperature', 'units': 'Cel'},
                { 'name': 'humidity', 'units': '%RH'},
                { 'name': 'igrometer_percent', 'units': '/100'},
                { 'name': 'ldr_percent', 'units': '/100'},
                { 'name': 'mq135_percent', 'units': '/100'},
                { 'name': 'pressure', 'units': 'Pa'},
                { 'name': 'rain_meter_percent', 'units': '/100'},
                { 'name': 'temperature', 'units': 'Cel'},
            ],
            "values": values_list
        }
    }

    # encode in JSON
    encoded = json.dumps(data)
    # sign message
    signature = hmac.new(bytes(HMAC_KEY, 'utf-8'), msg = encoded.encode('utf-8'), digestmod = hashlib.sha256).hexdigest()
    # set the signature again in the message, and encode again
    data['signature'] = signature
    encoded = json.dumps(data)
    # and upload the file
    res = requests.post(url=f'https://ingestion.edgeimpulse.com/api/{dataset}/data',
                        data=encoded,
                        headers={
                            'Content-Type': 'application/json',
                            'x-file-name': 'idle01',
                            'x-label': str(data_line['label']),
                            'x-api-key': API_KEY
                        })
    if (res.status_code == 200):
        print('Uploaded file to Edge Impulse', res.status_code, res.content)
    else:
        print('Failed to upload file to Edge Impulse', res.status_code, res.content)

def batch_upload(data, dataset):
    for idx, d in enumerate(data):
        upload_data(d, dataset)
        print(f"[{idx}/{len(data)}]")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"USAGE:", file=sys.stderr)
        print(f"  {sys.argv[0]} <data.csv>", file=sys.stderr)
        exit(1)
    input_path = sys.argv[1]
    train_out_path = input_path[:len(input_path)-4]+'_training.csv'
    test_out_path = input_path[:len(input_path)-4]+'_test.csv'
    
    # Convert the data in the correct format
    data = create_data(input_path)
    # Add to each data a label with the mean temp 2 hour from it
    for i,v in enumerate(data):
        future_element = data[(i+8) % len(data)]
        label = (float(future_element["bmp_temperature"]) + float(future_element["temperature"])) / 2
        data[i]["label"] = label
    # Split the data in training and testing set
    percent = int(len(data) * 80 / 100)
    random.shuffle(data)
    training_data = data[:percent]
    testing_data = data[percent:]
    # Upload dataset to edge impulse
    batch_upload(training_data, "training")
    batch_upload(testing_data, "testing")
    #[upload_data(d, "testing") for d in testing_data]
    #[upload_data(d, "training") for d in data[:5]]
    
    with open(train_out_path, "w+") as of:
        dw = csv.DictWriter(of, training_data[0].keys())
        dw.writeheader()
        dw.writerows(training_data)
    with open(test_out_path, "w+") as of:
        dw = csv.DictWriter(of, testing_data[0].keys())
        dw.writeheader()
        dw.writerows(testing_data)
        
    