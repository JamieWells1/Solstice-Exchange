#!/bin/bash
set -e

if [ -z "$VIRTUAL_ENV" ]; then
    source .venv/bin/activate
fi

# Only rebuild if --rebuild flag is passed
if [ "$1" == "--rebuild" ]; then
    echo "Rebuilding C++ extension..."
    pip install -e . -q
fi

python src/strategy/entry_point.py
