#!/usr/bin/env python3
"""
Simple WebSocket client to test the Solstice LOB Broadcaster

Usage:
    python3 websocket_client.py [TICKER]

Examples:
    python3 websocket_client.py           # Show all tickers
    python3 websocket_client.py AAPL      # Filter to AAPL only
    python3 websocket_client.py MSFT      # Filter to MSFT only
"""
import asyncio
import websockets
import json
import sys

async def test_broadcaster(ticker_filter=None):
    uri = "ws://localhost:8080"

    try:
        async with websockets.connect(uri) as websocket:
            if ticker_filter:
                print(f"Listening on {uri} for {ticker_filter}\n")
            else:
                print(f"Listening on {uri} for all tickers\n")

            while True:
                try:
                    message = await websocket.recv()
                    data = json.loads(message)

                    # Filter by ticker if specified
                    if ticker_filter and data.get('symbol') != ticker_filter:
                        continue

                    msg_type = data.get('type', 'unknown')

                    if msg_type == 'trade':
                        print(f"[TRADE] {data['symbol']} @ ${data['price']:.2f} x {data['quantity']}")
                        print(f"  Transaction ID: {data['transaction_id']}")
                        print(f"  Bid UID: {data['bid_uid']}, Ask UID: {data['ask_uid']}")

                    elif msg_type == 'order':
                        status = "MATCHED" if data['matched'] else "OPEN"
                        print(f"[ORDER] {status} - {data['symbol']} {data['side']}")
                        print(f"  Order ID: {data['order_id']}")
                        print(f"  Price: ${data['price']:.2f}, Qty: {data['quantity']}")

                    elif msg_type == 'book':
                        bid = f"${data['best_bid']:.2f}" if data['best_bid'] else "N/A"
                        ask = f"${data['best_ask']:.2f}" if data['best_ask'] else "N/A"
                        print(f"[BOOK] {data['symbol']} - Bid: {bid}, Ask: {ask}")

                    else:
                        print(f"[{msg_type.upper()}] {json.dumps(data, indent=2)}\n")

                except json.JSONDecodeError:
                    print(f"Received non-JSON message: {message}")

    except ConnectionRefusedError:
        print("Error: Could not connect to WebSocket server.")
        print("Make sure the Solstice broadcaster is running.")
    except KeyboardInterrupt:
        print("\nDisconnected.")

if __name__ == "__main__":
    ticker = sys.argv[1].upper() if len(sys.argv) > 1 else None
    try:
        asyncio.run(test_broadcaster(ticker))
    except KeyboardInterrupt:
        pass
