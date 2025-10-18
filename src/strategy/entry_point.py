from config.backtest.data_config import Config
from strategy.py_interface import PyInterface

import yfinance as yf


def fetch_data(cfg: Config) -> dict[str, list[float]]:

    ticker = yf.Ticker(cfg.ticker)

    if cfg.time_frame == "period":
        df = ticker.history(period=cfg.period, interval=cfg.interval)
    else:
        df = ticker.history(
            start=cfg.start_date, end=cfg.end_date, interval=cfg.interval
        )

    data = {
        "opens": df["Open"].tolist(),
        "highs": df["High"].tolist(),
        "lows": df["Low"].tolist(),
        "closes": df["Close"].tolist(),
        "volume": df["Volume"].tolist(),
        "timestamps": [int(ts.timestamp() * 1000) for ts in df.index],
    }

    data_missing: list[str] = []

    if not data["open"]:
        data_missing.append("Opens")
    if not data["high"]:
        data_missing.append("Highs")
    if not data["low"]:
        data_missing.append("Lows")
    if not data["close"]:
        data_missing.append("Closes")
    if not data["volume"]:
        data_missing.append("Volumes")
    if not data["timestamps"]:
        data_missing.append("Timestamps")

    if data_missing:
        raise ValueError(f"Misssing column in market data: {data_missing[0]}")

    return data


def main():
    data_cfg = Config()
    data = fetch_data(data_cfg)

    interface: PyInterface = PyInterface.establish()
    interface.orchestrate(data)
