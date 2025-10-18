from enum import Enum
import json
from pathlib import Path


CONFIG_FILE_PATH = Path(__file__).parent / "data_config.json"


class Interval(Enum):
    """Valid yfinance intervals"""

    ONE_MIN = "1m"  # 1 minute (max 7 days of data)
    TWO_MIN = "2m"  # 2 minutes (max 60 days)
    FIVE_MIN = "5m"  # 5 minutes (max 60 days)
    FIFTEEN_MIN = "15m"  # 15 minutes (max 60 days)
    THIRTY_MIN = "30m"  # 30 minutes (max 60 days)
    ONE_HOUR = "1h"  # 1 hour (max 730 days)
    ONE_DAY = "1d"  # 1 day (no limit)
    ONE_WEEK = "1wk"  # 1 week (no limit)
    ONE_MONTH = "1mo"  # 1 month (no limit)


class Period(Enum):
    """Valid yfinance periods"""

    ONE_DAY = "1d"
    FIVE_DAYS = "5d"
    ONE_MONTH = "1mo"
    THREE_MONTHS = "3mo"
    SIX_MONTHS = "6mo"
    ONE_YEAR = "1y"
    TWO_YEARS = "2y"
    FIVE_YEARS = "5y"
    TEN_YEARS = "10y"
    YTD = "ytd"
    MAX = "max"


class TimeFrame(Enum):
    """Valid time frames"""

    PERIOD = "Period"
    INTERVAL = "Interval"


class Config:
    def __init__(self) -> None:
        data = load_config_from_json(str(CONFIG_FILE_PATH))

        self.ticker = data["ticker"]
        self.interval = data["interval"]

        self.time_frame = data["time"]["time_frame"]

        if self.time_frame == "period":
            self.period = data["time"]["period"]

        elif self.time_frame == "start_end":
            self.start_date = data["time"]["start_end"]["start_date"]
            self.end_date = data["time"]["start_end"]["end_date"]

        else:
            valid_time_frames = [time_frame.value for time_frame in TimeFrame]
            raise ValueError(
                f"Invalid time frame '{self.time_frame}' specified. Must be one of: {valid_time_frames}"
            )

        is_valid: dict[bool, str] = validate(data)
        if is_valid.get(False):
            raise ValueError(is_valid[False])


def load_config_from_json(filepath: str):
    """Load from JSON file"""

    with open(filepath) as f:
        return json.load(f)


def validate(config) -> dict[bool, str]:
    """Ensure config is valid"""

    valid_intervals = [interval.value for interval in Interval]

    interval: str = config.get("interval")
    period: str = config.get("period")
    start_date: str = config.get("start_date")
    end_date: str = config.get("end_date")

    if interval not in valid_intervals:
        return {
            False: f"Invalid interval '{interval}' specified. Must be one of: {valid_intervals}"
        }

    if period and (start_date or end_date):
        return {False: "Use either 'period' or 'start/end', not both"}

    if (start_date and not end_date) or (end_date and not start_date):
        return {False: "Must specify both 'start' and 'end' if using dates"}

    if period:
        valid_periods = [period.value for period in Period]
        if period not in valid_periods:
            return {
                False: f"Invalid period '{period}' specified. Must be one of: {valid_periods}"
            }

    return {True: ""}
