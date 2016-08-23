#!/usr/bin/python

import os

if __name__=='__main__':
	os.system('./mcast_client &')
	os.sleep(1000)
	os.system('./mcast_server &')
