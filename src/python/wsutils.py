import os

standard_service_header = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<Response>\n"
terminal_begin = "<Terminal><Name>"
terminal_middle = "</Name><Value>"
terminal_end = "</Value></Terminal>\n"
standard_service_footer = "</Response>"

infrastructure_files = ['LVWSSysAdmin', 'server.py', 'commandserver.py', 'commandserver.pyc', 'wsutils.py', 'wsutils.pyc']
global_services_header = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<UserServices>\n"
global_services_footer = "</UserServices>"
def generate_user_services_file():
	all_files = os.listdir(".")
#	for f in infrastructure_files:
#		all_files.remove(f)
	services = global_services_header
	f = open("./LVWSSysAdmin/GetAllUserServices", "w")
	for s in all_files:
		services = services+"<WSName>"+s+"</WSName>\n"
	f.write(services+global_services_footer)
	f.close()
