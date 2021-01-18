# -*- coding: utf-8 -*-

import os, sys, re, codecs, binascii, cgi, cgitb, datetime, pickle
from msg import *
import json

cgitb.enable()
sys.stdout = codecs.getwriter('utf-8')(sys.stdout.detach())

def main():
	q = cgi.FieldStorage()
	
	print("Content-type: application-json; charset=utf-8\n\n")
	r = {'ids': [["one" , 1], ["two", 2]]}
	print(json.dumps(r))

        
main()
