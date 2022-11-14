#!/usr/bin/python3
from scapy.all import *
import base64
print(base64.b64decode(bytes(sorted(sniff(filter="tcp and host 140.113.213.213",count=10000),key=lambda x: x.ttl,reverse=True)[0].payload.payload.payload)).decode('utf-8'))
