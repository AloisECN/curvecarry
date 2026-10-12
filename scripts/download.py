import os, sys, requests, pandas as pd

KEY = os.environ.get("EIA_API_KEY")
if not KEY:
    sys.exit("Set EIA_API_KEY first")

BASE = "https://api.eia.gov/v2"

# market -> (API route, series IDs) I got it from EIA website not intiutive !

MARKETS = {
    "wti":  ("petroleum/pri/fut", [f"RCLC{i}" for i in range(1, 5)]),
    "ho":   ("petroleum/pri/fut", [f"EER_EPD2F_PE{i}_Y35NY_DPG" for i in range(1, 5)]),
    "rbob": ("petroleum/pri/fut", [f"EER_EPMRR_PE{i}_Y35NY_DPG" for i in range(1, 5)]),
    "ng":   ("natural-gas/pri/fut", [f"RNGC{i}" for i in range(1, 5)]),
}
def fetch(route, series):
    rows, offset = [], 0
    while True:
        r = requests.get(f"{BASE}/{route}/data/", params={
            "api_key": KEY, "frequency": "daily",
            "data[0]": "value", "facets[series][]": series,
            "sort[0][column]": "period", "sort[0][direction]": "asc",
            "offset": offset, "length": 5000,
        }, timeout=60)
        r.raise_for_status()
        batch = r.json()["response"]["data"]
        if not batch:
            break
        rows += batch
        offset += 5000
    return pd.DataFrame(rows)

os.makedirs("data", exist_ok=True)
for name, (route, ids) in MARKETS.items():
    frames = []
    for sid in ids:
        df = fetch(route, sid)
        if df.empty:
            print(f"WARNING: no data for {sid}, check the series ID")
            continue
        frames.append(pd.to_numeric(df.set_index("period")["value"].rename(sid)))
    if frames:
        out = pd.concat(frames, axis=1).sort_index()
        out.index.name = "date"
        out.to_csv(f"data/{name}.csv")
        print(f"{name}: {out.index[0]} -> {out.index[-1]}, {len(out)} rows")