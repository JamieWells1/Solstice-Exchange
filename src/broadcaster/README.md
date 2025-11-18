# Solstice WebSocket Broadcaster

Real-time market data broadcasting component built on Boost.Beast and Boost.Asio. Streams trade executions, order updates, and book changes to connected WebSocket clients with async message queuing and multi-threaded session management.

> ## [See Benchmark History]([link to benchmark history])

## Key Features

- WebSocket server broadcasting trades, orders, and book updates in JSON format
- Async message queuing with dedicated broadcast worker thread to prevent blocking
- Thread-safe session management with weak pointer cleanup
- Configurable order broadcast sampling via `broadcastInterval` to reduce traffic
- Non-blocking broadcast with `try_to_lock` mechanism for high-frequency order updates

---

## Local Execution

To enable the broadcaster:

Set `d_enableBroadcaster` to `true` in `config.h`

Then to run the executable:

`./build/bin/solstice`

---

## Benchmarks

### [Benchmark Name] - v[X.X] | [Full release]([link to release])

[Brief description of this benchmark version and what it represents]

**Config:**

- [Config parameter 1]
- [Config parameter 2]
- [Config parameter 3]
- [Config parameter 4]

**Result:**

- [Metric 1]: [Value]
- [Metric 2]: [Value]
- [Metric 3]: [Value]

---

### Plotting [Metric A] Against [Metric B]

[Hypothesis statement about the relationship between the two metrics]

| [Column 1 Header] | [Column 2 Header] | [Column 3 Header] |
| ----------------- | ----------------- | ----------------- |
| [Value]           | [Value]           | [Value]           |
| [Value]           | [Value]           | [Value]           |
| [Value]           | [Value]           | [Value]           |
| [Value]           | [Value]           | [Value]           |
| [Value]           | [Value]           | [Value]           |
| [Value]           | [Value]           | [Value]           |
| [Value]           | [Value]           | [Value]           |
| [Value]           | [Value]           | [Value]           |
| [Value]           | [Value]           | [Value]           |
| [Value]           | [Value]           | [Value]           |

**Plotted on a graph:**

> [Description of what the axes represent]

![[Image Title]]([path to image])

![[Image Title]]([path to image])

[Analysis paragraph explaining the results, whether the hypothesis was correct, and what the data shows]

### Plotting [Metric C] Against [Metric D]

[Hypothesis statement about the relationship between these metrics, including expected patterns and reasoning]

> [Note about test configuration or methodology]

| [Column 1 Header] | [Column 2 Header] | [Column 3 Header] |
| ----------------- | ----------------- | ----------------- |
| [Value]           | [Value]           | [Value]           |
| [Value]           | [Value]           | [Value]           |
| [Value]           | [Value]           | [Value]           |
| [Value]           | [Value]           | [Value]           |
| [Value]           | [Value]           | [Value]           |
| [Value]           | [Value]           | [Value]           |
| [Value]           | [Value]           | [Value]           |
| [Value]           | [Value]           | [Value]           |
| [Value]           | [Value]           | [Value]           |
| [Value]           | [Value]           | [Value]           |

**Plotted on a graph:**

> [Description of what the axes represent]

[Description of which subset of data this graph shows]:

![[Image Title]]([path to image])

[Description of what this alternate view shows]:

![[Image Title]]([path to image])

[Detailed analysis paragraph explaining patterns in the data, unexpected results, and technical reasoning behind the observations. Include discussion of any anomalies and potential causes.]
