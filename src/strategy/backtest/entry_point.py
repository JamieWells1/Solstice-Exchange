from .config import Config
from strategy import py_interface

import yfinance as yf


py_interface.Interface()

def fetch_data(cfg: Config):

    ticker = yf.Ticker(cfg.ticker)

    if cfg.time_frame == "period":
        df = ticker.history(period=cfg.period, interval=cfg.interval)
    else:
        df = ticker.history(
            start=cfg.start_date, end=cfg.end_date, interval=cfg.interval
        )

    return df


data_cfg = Config()
df = fetch_data(data_cfg)

print(df.head())
