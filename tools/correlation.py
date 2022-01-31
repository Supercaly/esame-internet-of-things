import sys
import numpy as np
from scipy.stats import pearsonr
from scipy.stats import spearmanr
from scipy.stats import mode
from matplotlib import pyplot
import seaborn as sns
import sens

def get_data(path, f):
    data = sens.create_data(path, f)
    dlist = list(map(list, zip(*map(lambda e: e.values(), data))))
    res = {}
    for i,k in enumerate(data[0].keys()):
        if k == "timestamp":
            res[k] = np.array(dlist[i])
        else:
            res[k] = np.array(dlist[i]).astype(float)
    return res
    

def split_day_night(data):
    dayl = list(filter(lambda e: e["ldr_percent"] >= 50.0, [dict(zip(data,t)) for t in zip(*data.values())]))
    nightl = list(filter(lambda e: e["ldr_percent"] < 50.0, [dict(zip(data,t)) for t in zip(*data.values())]))
    return (
        {k: [dic[k] for dic in dayl] for k in dayl[0]},
        {k: [dic[k] for dic in nightl] for k in nightl[0]},
    )


def compute_cov(a1, a2):
    cov = np.cov(a1, a2)
    pcov,_ = pearsonr(a1, a2)
    scov,_ = spearmanr(a1, a2)
    return (cov,pcov,scov)


def compute_cov_matrix(cov):
    l1 = list()
    for _,v1 in cov.items():
        l2 = list()
        for _,v2 in cov.items():
            _,pcov,scov = compute_cov(v1,v2)
            l2.append(scov)
        l1.append(l2)
    return np.matrix(l1)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"USAGE:", file=sys.stderr)
        print(f"  {sys.argv[0]} <data.csv>", file=sys.stderr)
        exit(1)
    input_path = sys.argv[1]

    data = get_data(input_path, sens.filter_fields)
    data.pop("timestamp", None)

    print(f"")
    print(f"General statistics:")
    for k,v in data.items():
        print(f"  {k}:")
        print(f"    mean: {np.mean(v)}, std: {np.std(v)}, median: {np.median(v)}, mode: {mode(v)}")
    print(f"")

    # pyplot.scatter(temperature, bmp_temperature)
    # pyplot.show()

    day,night = split_day_night(data)

    total_len = len(data["temperature"])
    day_len = len(day["temperature"])
    night_len = len(night["temperature"])

    print(f"")
    print(f"Day samples: {day_len}({(day_len*100)/total_len}%)")
    print(f"Night samples: {night_len}({(night_len*100)/total_len}%)")
    print(f"")

    print(f"")
    print(f"Day statistics:")
    for k,v in day.items():
        print(f"  {k}:")
        print(f"    mean: {np.mean(v)}, std: {np.std(v)}, median: {np.median(v)}, mode: {mode(v)}, min: {np.min(v)}, max: {np.max(v)}")
    print(f"")
    print(f"")
    print(f"Night statistics:")
    for k,v in night.items():
        print(f"  {k}:")
        print(f"    mean: {np.mean(v)}, std: {np.std(v)}, median: {np.median(v)}, mode: {mode(v)}, min: {np.min(v)}, max: {np.max(v)}")
    print(f"")

    
    sorted_data = {}
    sorted_data["temperature"] = data["temperature"]
    sorted_data["bmp_temperature"] = data["bmp_temperature"]
    sorted_data["humidity"] = data["humidity"]
    sorted_data["ldr_percent"] = data["ldr_percent"]
    sorted_data["igrometer_percent"] = data["igrometer_percent"]
    sorted_data["rain_meter_percent"] = data["rain_meter_percent"]
    sorted_data["mq135_percent"] = data["mq135_percent"]
    sorted_data["pressure"] = data["pressure"]
    sorted_data["altitude"] = data["altitude"]

    covp = compute_cov_matrix(sorted_data)
    print(covp)
    print("")
    for k1,v1 in sorted_data.items():
        for k2,v2 in sorted_data.items():
            cov,pcov,scov = compute_cov(v1,v2)
            print(f"Covariance {k1},{k2}: {pcov} {scov}")
    print("")

    labels = ["DHT11 temp.","BMP180 temp.","umidità","LDR%","igro.%","pioggia%","MQ-135%","press.","alt."]
    print(labels)
    print(sorted_data.keys())
    
    sns.heatmap(covp, cbar=True, annot=True, square=True, cmap='coolwarm',
        xticklabels=labels, yticklabels=labels)
    pyplot.tight_layout()
    pyplot.show()