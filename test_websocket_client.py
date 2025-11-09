#!/usr/bin/env python3
"""
Simple WebSocket client to test the Solstice LOB Broadcaster
"""
import asyncio
import websockets
import json

async def test_broadcaster():
    uri = "ws://localhost:8080"

    print(f"Connecting to {uri}...")

    try:
        async with websockets.connect(uri) as websocket:
            print("Connected! Listening for messages...")
            print("Press Ctrl+C to stop\n")

            while True:
                try:
                    message = await websocket.recv()
                    data = json.loads(message)

                    # Pretty print the message
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

                    elif msg_type == 'book_update':
                        bid = f"${data['best_bid']:.2f}" if data['best_bid'] else "N/A"
                        ask = f"${data['best_ask']:.2f}" if data['best_ask'] else "N/A"
                        print(f"[BOOK] {data['symbol']} - Bid: {bid}, Ask: {ask}")

                    else:
                        print(f"[{msg_type.upper()}] {json.dumps(data, indent=2)}")

                    print()  # Empty line for readability

                except json.JSONDecodeError:
                    print(f"Received non-JSON message: {message}")

    except ConnectionRefusedError:
        print("Error: Could not connect to WebSocket server.")
        print("Make sure the Solstice broadcaster is running.")
    except KeyboardInterrupt:
        print("\nDisconnected.")

if __name__ == "__main__":
    try:
        asyncio.run(test_broadcaster())
    except KeyboardInterrupt:
        pass
