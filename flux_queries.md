# InfluxDB queries

This document contains all the InfluxDB queries used to generate the plots

## Compute means for every day

```
data = from(bucket: "test")
  |> range(start: 2021-12-23T00:00:00.0Z, stop: 2022-01-17T16:00:00.0Z)
  |> filter(fn: (r) => 
    r._measurement == "data_calisti")
  |> filter(fn: (r) =>
    r._field == "altitude" or r._field == "bmp_temperature" or r._field == "humidity" or r._field == "igrometer_percent" 
    or r._field == "ldr_percent" or r._field == "mq135_percent" or r._field == "rain_meter_percent" or r._field == "pressure" 
    or r._field == "temperature")
  |> aggregateWindow(every: 1d, fn: mean)
```

## Compute anomalies

```
unpivot = (tab=<-) =>
  union(tables: [
    tab |> map(fn: (r) => ({_start:r._start,_stop:r._stop,_time:r._time,_field:"altitude",_value:r.altitude})),
    tab |> map(fn: (r) => ({_start:r._start,_stop:r._stop,_time:r._time,_field:"anomaly_score",_value:r.anomaly_score})),
    tab |> map(fn: (r) => ({_start:r._start,_stop:r._stop,_time:r._time,_field:"bmp_temperature",_value:r.bmp_temperature})),
    tab |> map(fn: (r) => ({_start:r._start,_stop:r._stop,_time:r._time,_field:"humidity",_value:r.humidity})),
    tab |> map(fn: (r) => ({_start:r._start,_stop:r._stop,_time:r._time,_field:"igrometer_percent",_value:r.igrometer_percent})),
    tab |> map(fn: (r) => ({_start:r._start,_stop:r._stop,_time:r._time,_field:"rain_meter_percent",_value:r.rain_meter_percent})),
    tab |> map(fn: (r) => ({_start:r._start,_stop:r._stop,_time:r._time,_field:"ldr_percent",_value:r.ldr_percent})),
    tab |> map(fn: (r) => ({_start:r._start,_stop:r._stop,_time:r._time,_field:"mq135_percent",_value:r.mq135_percent})),
    tab |> map(fn: (r) => ({_start:r._start,_stop:r._stop,_time:r._time,_field:"pressure",_value:r.pressure})),
    tab |> map(fn: (r) => ({_start:r._start,_stop:r._stop,_time:r._time,_field:"temperature",_value:r.temperature})),
  ])|> group(columns: ["_field"])


data = from(bucket: "test")
  |> range(start: 2022-01-20T10:00:00.0Z, stop: v.timeRangeStop)
  |> filter(fn: (r) => r["_measurement"] == "data_calisti")
  |> filter(fn: (r) => r["_field"] == "anomaly_score" or r._field == "altitude" or r._field == "bmp_temperature" or 
    r._field == "humidity" or r._field == "igrometer_percent" or r._field == "ldr_percent" or r._field == "mq135_percent" or 
    r._field == "rain_meter_percent" or r._field == "pressure" or r._field == "temperature")
  |> toFloat()

anomaly = data |> pivot(rowKey:["_time"], columnKey: ["_field"], valueColumn: "_value")
  |> filter(fn: (r) => r.anomaly_score >= 0)
  |> unpivot()
  |> filter(fn: (r) => r._field=="anomaly_score" or r._field=="temperature" or r._field=="bmp_temperature" or r._field=="humidity")
  |> yield()
```