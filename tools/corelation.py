import sys
import numpy as np
from scipy.stats import pearsonr
from scipy.stats import spearmanr
from scipy.stats import mode
from matplotlib import pyplot
import sens

temperature = "temperature"
humidity = "humidity"
rain_meter_percent = "rain_meter_percent"
igrometer_percent = "igrometer_percent"
ldr_percent = "ldr_percent"
mq135_percent = "mq135_percent"
pressure = "pressure"
bmp_temperature = "bmp_temperature"
altitude = "altitude"

def get_data(path):
    data = sens.create_data(path)
    temperature = list()
    humidity = list()
    rain_meter_percent = list()
    igrometer_percent = list()
    ldr_percent = list()
    mq135_percent = list()
    pressure = list()
    bmp_temperature = list()
    altitude = list()
    for i,v in enumerate(data):
        temperature.append(float(v["temperature"]))
        humidity.append(float(v["humidity"]))
        rain_meter_percent.append(float(v["rain_meter_percent"]))
        igrometer_percent.append(float(v["igrometer_percent"]))
        ldr_percent.append(float(v["ldr_percent"]))
        mq135_percent.append(float(v["mq135_percent"]))
        pressure.append(float(v["pressure"]))
        bmp_temperature.append(float(v["bmp_temperature"]))
        altitude.append(float(v["altitude"]))
    temperature = np.array(temperature)
    humidity = np.array(humidity)
    rain_meter_percent = np.array(rain_meter_percent)
    igrometer_percent = np.array(igrometer_percent)
    ldr_percent = np.array(ldr_percent)
    mq135_percent = np.array(mq135_percent)
    pressure = np.array(pressure)
    bmp_temperature = np.array(bmp_temperature)
    altitude = np.array(altitude)
    return {
        'temperature': temperature,
        'humidity': humidity,
        'rain_meter_percent': rain_meter_percent,
        'igrometer_percent': igrometer_percent,
        'ldr_percent': ldr_percent,
        'mq135_percent': mq135_percent,
        'pressure': pressure,
        'bmp_temperature': bmp_temperature,
        'altitude': altitude
    }


def split_day_night(data):
    s = data["temperature"].shape
    day = {
        'temperature': list(),
        'humidity': list(),
        'rain_meter_percent': list(),
        'igrometer_percent': list(),
        'ldr_percent': list(),
        'mq135_percent': list(),
        'pressure': list(),
        'bmp_temperature': list(),
        'altitude': list()
    }
    night = {
        'temperature': list(),
        'humidity': list(),
        'rain_meter_percent': list(),
        'igrometer_percent': list(),
        'ldr_percent': list(),
        'mq135_percent': list(),
        'pressure': list(),
        'bmp_temperature': list(),
        'altitude': list()
    }
    for idx in range(0,len(data[temperature])):
        if data["ldr_percent"][idx] >= 50.0:
            day['temperature'].append(data['temperature'][idx])
            day['humidity'].append(data['humidity'][idx])
            day['rain_meter_percent'].append(data['rain_meter_percent'][idx])
            day['igrometer_percent'].append(data['igrometer_percent'][idx])
            day['ldr_percent'].append(data['ldr_percent'][idx])
            day['mq135_percent'].append(data['mq135_percent'][idx])
            day['pressure'].append(data['pressure'][idx])
            day['bmp_temperature'].append(data['bmp_temperature'][idx])
            day['altitude'].append(data['altitude'][idx])
        else:
            night['temperature'].append(data['temperature'][idx])
            night['humidity'].append(data['humidity'][idx])
            night['rain_meter_percent'].append(data['rain_meter_percent'][idx])
            night['igrometer_percent'].append(data['igrometer_percent'][idx])
            night['ldr_percent'].append(data['ldr_percent'][idx])
            night['mq135_percent'].append(data['mq135_percent'][idx])
            night['pressure'].append(data['pressure'][idx])
            night['bmp_temperature'].append(data['bmp_temperature'][idx])
            night['altitude'].append(data['altitude'][idx])

    day['temperature'] = np.array(day['temperature'])
    day['humidity'] = np.array(day['humidity'])
    day['rain_meter_percent'] = np.array(day['rain_meter_percent'])
    day['igrometer_percent'] = np.array(day['igrometer_percent'])
    day['ldr_percent'] = np.array(day['ldr_percent'])
    day['mq135_percent'] = np.array(day['mq135_percent'])
    day['pressure'] = np.array(day['pressure'])
    day['bmp_temperature'] = np.array(day['bmp_temperature'])
    day['altitude'] = np.array(day['altitude'])
    night['temperature'] = np.array(night['temperature'])
    night['humidity'] = np.array(night['humidity'])
    night['rain_meter_percent'] = np.array(night['rain_meter_percent'])
    night['igrometer_percent'] = np.array(night['igrometer_percent'])
    night['ldr_percent'] = np.array(night['ldr_percent'])
    night['mq135_percent'] = np.array(night['mq135_percent'])
    night['pressure'] = np.array(night['pressure'])
    night['bmp_temperature'] = np.array(night['bmp_temperature'])
    night['altitude'] = np.array(night['altitude'])
    return (day,night)


def compute_cov(a1, a2):
    cov = np.cov(a1, a2)
    pcov,_ = pearsonr(a1, a2)
    scov,_ = spearmanr(a1, a2)
    return (cov,pcov,scov)


def compute_cov_matrix(cov):
    l1 = list()
    for _,v1 in cov.items():
        l2 = list()
        for _,v2 in data.items():
            _,_,scov = compute_cov(v1,v2)
            l2.append(scov)
        l1.append(l2)
    return np.matrix(l1)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"USAGE:", file=sys.stderr)
        print(f"  {sys.argv[0]} <data.csv>", file=sys.stderr)
        exit(1)
    input_path = sys.argv[1]

    data = get_data(input_path)

    print(f"")
    print(f"General statistics:")
    for k,v in data.items():
        print(f"  {k}:")
        print(f"    mean: {np.mean(v)}, std: {np.std(v)}, median: {np.median(v)}, mode: {mode(v)}")
    print(f"")

    # pyplot.scatter(temperature, bmp_temperature)
    # pyplot.show()

    day,night = split_day_night(data)

    total_len = len(data[temperature])
    day_len = len(day[temperature])
    night_len = len(night[temperature])

    print(f"")
    print(f"Day samples: {day_len}({(day_len*100)/total_len}%)")
    print(f"Night samples: {night_len}({(night_len*100)/total_len}%)")
    print(f"")

    print(f"")
    print(f"Day statistics:")
    for k,v in day.items():
        print(f"  {k}:")
        print(f"    mean: {np.mean(v)}, std: {np.std(v)}, median: {np.median(v)}, mode: {mode(v)}")
    print(f"")
    print(f"")
    print(f"Night statistics:")
    for k,v in night.items():
        print(f"  {k}:")
        print(f"    mean: {np.mean(v)}, std: {np.std(v)}, median: {np.median(v)}, mode: {mode(v)}")
    print(f"")

    print(compute_cov_matrix(data))
    # print("")
    # for k1,v1 in data.items():
    #     for k2,v2 in data.items():
    #         cov,pcov,scov = compute_cov(v1,v2)
    #         print(f"Covariance {k1},{k2}: {scov}")
    # print("")