import wsutils
from random import randint

def current_command(direction, magnitude, lights):
	directionString = "IDLE"
	if direction == 1:
		directionString = "FORWARD"
	elif direction == 2:
		directionString = "REVERSE"
	elif direction & 4:
		directionString = "LEFT"
	elif direction & 8:
		directionString = "RIGHT"
	updated_html = wsutils.standard_service_header
	updated_html = updated_html+wsutils.terminal_begin+"direction"+wsutils.terminal_middle+str(direction)+wsutils.terminal_end
	updated_html = updated_html+wsutils.terminal_begin+"directionString"+wsutils.terminal_middle+directionString+wsutils.terminal_end
	updated_html = updated_html+wsutils.terminal_begin+"magnitude"+wsutils.terminal_middle+str(magnitude)+wsutils.terminal_end
	updated_html = updated_html+wsutils.terminal_begin+"lights"+wsutils.terminal_middle+str(1 if lights else 0)+wsutils.terminal_end
	updated_html = updated_html+wsutils.standard_service_footer
	f = open("./CommandServer/currentCommand", "w")
	f.write(updated_html)
	f.close()
	return
