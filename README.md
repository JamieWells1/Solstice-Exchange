# Solstice Matching Engine

Solstice is a high-performance, limit order book engine built in C++23. It is architected for experimentation with order flow, benchmarking, and execution algorithms. The engine prioritizes raw performance, extensibility, and minimal cognitive overhead.

## Key Features

- Fully custom matching logic with time-price priority.
- Modular components: `Order`, `Matcher`, `OrderBook`, `OrderProcessor`.
- Multi-threaded order processing with thread-safe ticker-level locking.
- Benchmark-mode ready via `goldpkg` execution.

---

## Local Execution

To run benchmarks locally:

```bash
./goldpkg/gold run
```

Execution parameters are configurable via `Config` (see `config.h`).

---

## Benchmarks

### Single-threaded Baseline - v0.1 | [Full release](https://github.com/JamieWells1/Solstice-Limit-Order-Book/releases/tag/v0.1)

Initial working version with synchronous order generation and processing.

**Config:**

- 100,000 orders
- 10 tickers
- Price range: $8–$10 (2 d.p.)
- Quantity: 1–20

**Result:**

- Matched: 49,788
- Time: 1,225 ms
- Throughput: ~81,600 orders/sec

---

### Latest Performance Optimisation Release (v0.2.0) | [Full release](https://github.com/JamieWells1/Solstice-Limit-Order-Book/releases/tag/v0.2.0)

**Config:**

- 100,000 orders
- 10 tickers
- Price range: $8–$10 (2 d.p.)
- Quantity: 1–20

**Result:**

- Matched: 54,652
- Time: 191 ms
- Throughput: ~523,600 orders/sec

### Plotting Orders Executed Against Execution Time

It's worth noting that I tried these benchmark performance tests on two different computers:

- A MacBook Air M3
- A MacBook Pro M4

Surprisingly, the M3 yielded better performance results, almost half the amount of time it took for the M4 to complete its executions. Because of this, I will be using the numbers I got when using the MacBook Pro M4.

My hypothesis is that orders executed and execution time will have a linear relationship, as there are no external factors at this number of requests which are expected to interfere with the program.

| Orders Executed | Execution Time (ms) | Throughput (orders/sec) |
| --------------- | ------------------- | ----------------------- |
| 100,000         | 405                 | 246,913                 |
| 200,000         | 797                 | 250,939                 |
| 300,000         | 1,195               | 251,046                 |
| 400,000         | 1,601               | 249,844                 |
| 500,000         | 1,994               | 250,251                 |
| 600,000         | 2,396               | 250,835                 |
| 700,000         | 2,782               | 251,596                 |
| 800,000         | 3,197               | 250,117                 |
| 900,000         | 3,588               | 250,418                 |
| 1,000,000       | 3,984               | 251,005                 |

**Plotted on a graph:**

> Execution time in milliseconds on the x-axis plotted against orders executed in hundreds of thousands on the y-axis for graphic simplicity.

![v0.2.0 Plot No Line](/assets/v0.2.0_plot_no_line.png)

![v0.2.0 Plot With Line](/assets/v0.2.0_plot_line.png)

As we can see, the number of orders executed is directly correlated to execution time in a linear fashion, as correctly stated in my hypothesis. These numbers were taken as an average over 5 tests across each level of orders executed. The throughput remained almost identical across all of the tests, with a slight dip at the 100,000 orders mark.

### Plotting Tickers Used Executed Against Execution Time

Another thing I wanted to measure was how execution time was affected when using a varied number of tickers. My hypothesis is that number of tickers used will be directly correlated to the **inverse** of execution time via a **logarithmic relationship**, due to threads being locked based on **ticker access**.

Each time a thread wants to access the book for a given ticker, a `mutex` locks the book at that given ticker, denying any other threads from accessing the book at that ticker. With a large number of threads, the latency due to two threads trying to access the same ticker at the same time is negligible, but as number of tickers decreases, the chances of that happening increase.

> For each test, I generated a constant number of orders of 100,000.

| Tickers Used    | Execution Time (ms) | Throughput (orders/sec) |
| --------------- | ------------------- | ----------------------- |
| 1               | 572                 | 174,825                 |
| 2               | 382                 | 261,780                 |
| 3               | 305                 | 327,869                 |
| 4               | 259                 | 386,102                 |
| 5               | 225                 | 444,444                 |
| 6               | 211                 | 473,933                 |
| 7               | 192                 | 520,833                 |
| 8               | 395                 | 253,164                 |
| 9               | 404                 | 247,525                 |
| 10              | 406                 | 246,305                 |

**Plotted on a graph:**

> Number of tickers on the x-axis plotted against deciseconds on the y-axis for graphic simplicity.

Graph plotting only points within regular pattern:

![v0.2.0 Ticker Plot Logarithmic](/assets/v0.2.0_ticker_plot_logarithmic.png)

Graph plotting all points:

![v0.2.0 Ticker Plot All](/assets/v0.2.0_ticker_plot_all.png)

Looking at the data, we can see that the graph is in classic logarithmic form, and can be expressed in the form `y = alog(x)+b`, with threads fighting over processing each ticker at lower numbers. However, at 8 tickers, the execution time drastically increases to almost double that of 7 tickers, the one just before.

After doing some research, I concluded that this is due to **over-parallelisation**. After 7 tickers, the **inflection point** is reached, and my hypothesis is that at this point, the threads processing orders can no longer keep up with order generation. Once the number of pending tasks starts to pile up, threads must ensure that when they go to pick up a new task, there are no other threads already going to or already picking up that task, which is a lot quicker and easier if all the other threads are still busy. This is ensured by locking the queue of mutexes whilst a task is being picked up from `orderProcessQueue` with the use of `std::lock_guard<std::mutex> lock(d_queueMutex)`.
