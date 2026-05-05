import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import re
from sklearn.linear_model import LinearRegression

df=pd.read_csv("log4.txt", sep=" ")
print(df.head())
print(df.columns)

df.columns = ["time", "t1", "t2", "t3", "p", "x", "y"]

def ConvertTemp(x):
    return (((((-1.4078e-12 * x + 3.8668e-09) * x - 4.2095e-06) * x + 2.2864e-03) * x - 7.2374e-01) * x + 1.5920e+02)
def clean_temp_mean(row):
    # Ignore the second thermistor (t2) because it is unreliable.
    values = [row["t1"], row["t3"]]
    values = [v for v in values if np.isfinite(v)]
    if not values:
        return np.nan
    if len(values) == 1:
        return values[0]

    median = np.median(values)
    filtered = [v for v in values if abs(v - median) < 5]
    if filtered:
        return sum(filtered) / len(filtered)
    return median

def pressure_to_altitude(pressure_pa, sea_level_hpa=1013.25):
    pressure_hpa = pressure_pa / 100.0
    return 44330.0 * (1.0 - (pressure_hpa / sea_level_hpa) ** 0.1903)

df["t_clean"] = df.apply(clean_temp_mean, axis=1)

x=df["time"]
df["t1"]=df["t1"].apply(ConvertTemp)
df["t2"]=df["t2"].apply(ConvertTemp)
df["t3"]=df["t3"].apply(ConvertTemp)
z=df["p"]
df["altitude"] = pressure_to_altitude(df["p"])
clean=[]
for _,row in df.iterrows():
    clean.append(clean_temp_mean(row))  
df["clean"]=clean

plt.clf()
plt.plot(x, df["t1"], label="Thermistor 1")
plt.plot(x, df["t3"], label="Thermistor 3")
plt.xlabel("time")
plt.ylabel("temperature (°C)")
plt.title("Thermistor comparison over time (ignoring t2)")
plt.legend()
plt.tight_layout()
plt.savefig("image_thermistors.png")

# temperature plots
plt.clf()
plt.plot(x, df["clean"], color="green", linestyle='-', label="Temperature (°C)")
plt.xlabel("time")
plt.ylabel("temperature (°C)")
plt.title("Temperature over time")
plt.legend()
plt.tight_layout()
plt.savefig("image_temp_raw.png")

model = LinearRegression()
X = x.values.reshape(-1, 1)
y = df["clean"].values
model.fit(X, y)
y_pred = model.predict(X)

plt.clf()
plt.plot(x, df["clean"], color="green", linestyle='-', alpha=0.4, label="Raw temperature")
plt.plot(x, y_pred, color="red", linestyle='--', label="Regression line")
plt.xlabel("time")
plt.ylabel("temperature (°C)")
plt.title("Temperature over time with linear regression")
plt.legend()
plt.tight_layout()
plt.savefig("image_temp_regression.png")
print(df.head())

plt.clf()
'''
baseline = df["p"].mean()
df["pc"] = df["p"] - baseline
'''
model = LinearRegression()

X = x.values.reshape(-1, 1)
y = df["p"].values

model.fit(X, y)

y_pred = model.predict(X)
plt.plot(x, y_pred, color="red", label="Regression Line")
plt.ticklabel_format(style='plain', axis='y')
plt.scatter(x,z)
plt.xlabel("time")
plt.ylabel("pressure")
plt.title("Variation of pressure over time, 100500Pa baseline")
plt.savefig("image1.png")

plt.clf()

model = LinearRegression()

X = x.values.reshape(-1, 1)
y = df["altitude"].values

model.fit(X, y)
y_pred = model.predict(X)

plt.plot(x, y_pred, color="red", label="Regression Line")
plt.scatter(x, df["altitude"], label="Altitude data")

plt.xlabel("time")
plt.ylabel("altitude (m)")
plt.title("Altitude over time (from pressure)")
plt.legend()
plt.tight_layout()
plt.savefig("image3.png")

plt.clf()
fig, ax1 = plt.subplots()
ax1.plot(x, df["clean"], color="green", linestyle="-", label="Temperature (°C)")
ax1.set_xlabel("time")
ax1.set_ylabel("temperature (°C)", color="green")
ax1.tick_params(axis="y", labelcolor="green")

ax2 = ax1.twinx()
ax2.plot(x, df["altitude"], color="tab:blue", linestyle="--", label="Altitude (m)")
ax2.set_ylabel("altitude (m)", color="tab:blue")
ax2.tick_params(axis="y", labelcolor="tab:blue")

fig.suptitle("Temperature and Altitude over Time")
ax1.legend(loc="upper left")
ax2.legend(loc="upper right")
fig.tight_layout()
fig.savefig("image_overlay.png")

plt.clf()
plt.scatter(df["altitude"], df["clean"], alpha=0.6, color="purple")
plt.xlabel("altitude (m)")
plt.ylabel("temperature (°C)")
plt.title("Temperature by Altitude")
plt.tight_layout()
plt.savefig("image_temp_by_altitude.png")