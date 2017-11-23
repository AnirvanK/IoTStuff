#!/usr/bin/env python
import web
#import time
#import xml.etree.ElementTree as ET
import sys, os

urls =('/tempsensor', 'tempsensor', '/ultrasonicsensor', 'ultrasonicsensor', '/motionsensor', 'motionsensor')

app = web.application(urls, globals())

class tempsensor:       
    def POST(self):
        print("tempsensor - Data received: " + web.data())

class ultrasonicsensor:
    def POST(self):
        print("Ultrasonic - Data received: " + web.data())

class motionsensor:
    def POST(self):
        print("motionsensor - Data received: " + web.data())        
        
if __name__ == "__main__":
    app.run()
