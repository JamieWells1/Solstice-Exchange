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

    if not data["opens"]:
        data_missing.append("Opens")
    if not data["highs"]:
        data_missing.append("Highs")
    if not data["lows"]:
        data_missing.append("Lows")
    if not data["closes"]:
        data_missing.append("Closes")
    if not data["volume"]:
        data_missing.append("Volumes")
    if not data["timestamps"]:
        data_missing.append("Timestamps")

    if data_missing:
        raise ValueError(f"Misssing column in market data: {data_missing[0]}")

    return data


def main():
    print("\nStarting backtest...")

    try:
        data_cfg = Config()
        print(f"Fetching data for {data_cfg.ticker}...")
        data = fetch_data(data_cfg)
        print(f"Fetched {len(data['closes'])} candles")

        print("Establishing interface...")
        interface: PyInterface = PyInterface.establish()

        print("Running strategy...")
        report = interface.orchestrate(data)

        print("Strategy complete!\n")

        print(f"Report: \nTrades completed: {report.tradesCompleted}\n")

    except Exception as e:
        print(f"Error: {e}")
        import traceback

        traceback.print_exc()


if __name__ == "__main__":
    main()
