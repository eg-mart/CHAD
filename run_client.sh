#!/bin/bash
python -m venv venv
source venv/bin/activate
pip install -r requirements.txt
python client_src/client.py
