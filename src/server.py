#!/usr/bin/env python
import sys, tty, termios, thread
import cgi, SocketServer, SimpleHTTPServer
import wsutils, commandserver

gDirection = 0;
gMagnitude = 0;

def getDirection():
	global gDirection
	global gMagnitude
        while(1):
		while(1):
			fd = sys.stdin.fileno()
			old_settings = termios.tcgetattr(fd)
			try:
				tty.setraw(sys.stdin.fileno())
				arrow = sys.stdin.read(3)
			finally:
				termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
			if arrow!='':break

		if arrow=='\x1b[A':
			if gDirection & 1 and gMagnitude < 120:
				gMagnitude += 3;
			gDirection = 1
		elif arrow=='\x1b[B':
			if gDirection & 2 and gMagnitude < 120:
				gMagnitude += 3;
			gDirection = 2
		elif arrow=='\x1b[D':
			gDirection |= 4
		elif arrow=='\x1b[C':
			gDirection |= 8

class Handler(SimpleHTTPServer.SimpleHTTPRequestHandler):
    def do_GET(self):
	global gDirection
	global gMagnitude
	if "/CommandServer/currentCommand" in self.path:
		commandserver.current_command(gDirection, gMagnitude)
		if gDirection == 0 and gMagnitude >= 24:
			gMagnitude -= 24
		elif gDirection == 0:
			gMagnitude = 0
		if gDirection < 3:
			gDirection = 0
		gDirection &= 3
        return SimpleHTTPServer.SimpleHTTPRequestHandler.do_GET(self)

    def end_headers(self):
        self.send_header("Access-Control-Allow-Origin", "*")
        SimpleHTTPServer.SimpleHTTPRequestHandler.end_headers(self)

wsutils.generate_user_services_file()
httpd = SocketServer.TCPServer(("", 8080), Handler)
print "serving at port", 8080
thread.start_new_thread(getDirection, ())
httpd.serve_forever()
