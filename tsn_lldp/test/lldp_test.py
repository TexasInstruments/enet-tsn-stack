import os
import signal
import subprocess
import time
import shutil
from robot.api import logger as _logger
from robot.errors import PassExecution

LLDP_OVIP_BASE_PORT=7000
INTERFACE0="cbeth0" #The interface name in yang file will be cbeth0_7000
INTERFACE1="cbeth1" #The interface name in yang file will be cbeth1_7001
INF_NAME0="cbeth0_7000"
INF_NAME1="cbeth1_7001"
DB0="lldp_db_cbeth0"
DB1="lldp_db_cbeth1"
uniconf1_pid=None
uniconf2_pid=None
lldp_pid0=None
lldp_pid1=None

def create_ietf_config_file(if_name, enable, oper_status, lldp_index):
	ovip_port=LLDP_OVIP_BASE_PORT+lldp_index
	file_name="ietf-"+if_name+".conf"
	print("create_ietf_config_file:", file_name)
	# ovip port can be 7001 & 7002
	str="/ietf-interfaces/interfaces/interface|name:{0}_{1}|/enabled {2}".format(if_name, ovip_port, enable)
	str+="\n"
	str+="/ietf-interfaces/interfaces/interface|name:{0}_{1}|/oper-status {2}".format(if_name, ovip_port, oper_status)

	f=open(file_name, "w")
	f.write(str)
	f.close

def create_lldp_config_file(interface_name, lldp_index, msg_fast_tx, msg_tx_hold, tx_interval, reinit_delay, tx_credit_max, tx_fast_init):
	# ovip port can be 7001 & 7002
	ovip_port=LLDP_OVIP_BASE_PORT+lldp_index
	last_mac=10+lldp_index
	file_name="lldp-"+interface_name+".conf"
	print("create_lldp_config_file:", file_name)
	str="/ieee802-dot1ab-lldp/lldp/message-fast-tx {}\n".format(msg_fast_tx)
	str+="/ieee802-dot1ab-lldp/lldp/message-tx-hold-multiplier {}\n".format(msg_tx_hold)
	str+="/ieee802-dot1ab-lldp/lldp/message-tx-interval {}\n".format(tx_interval)
	str+="/ieee802-dot1ab-lldp/lldp/reinit-delay {}\n".format(reinit_delay)
	str+="/ieee802-dot1ab-lldp/lldp/tx-credit-max {}\n".format(tx_credit_max)
	str+="/ieee802-dot1ab-lldp/lldp/tx-fast-init {}\n".format(tx_fast_init)
	str+="/ieee802-dot1ab-lldp/lldp/notification-interval 30\n"
	str+="/ieee802-dot1ab-lldp/lldp/local-system-data/chassis-id-subtype 4\n"
	str+="/ieee802-dot1ab-lldp/lldp/local-system-data/chassis-id 01-02-03-04-06-{}\n".format(last_mac)
	str+="/ieee802-dot1ab-lldp/lldp/local-system-data/system-name SystemName-{0}_{1}\n".format(interface_name, lldp_index)
	str+="/ieee802-dot1ab-lldp/lldp/local-system-data/system-capabilities-supported 11111111111\n"
	str+="/ieee802-dot1ab-lldp/lldp/local-system-data/system-capabilities-enabled 11110111011\n"
	str+="/ieee802-dot1ab-lldp/lldp/port|name:{0}_{1}|dest-mac-address:01-02-03-04-06-{2}|/admin-status 3\n".format(interface_name, ovip_port, last_mac)
	str+="/ieee802-dot1ab-lldp/lldp/port|name:{0}_{1}|dest-mac-address:01-02-03-04-06-{2}|/notification-enable true\n".format(interface_name, ovip_port, last_mac)
	str+="/ieee802-dot1ab-lldp/lldp/port|name:{0}_{1}|dest-mac-address:01-02-03-04-06-{2}|/tlvs-tx-enable 1111\n".format(interface_name, ovip_port, last_mac)
	str+="/ieee802-dot1ab-lldp/lldp/port|name:{0}_{1}|dest-mac-address:01-02-03-04-06-{2}|/port-id-subtype 3\n".format(interface_name, ovip_port, last_mac)
	str+="/ieee802-dot1ab-lldp/lldp/port|name:{0}_{1}|dest-mac-address:01-02-03-04-06-{2}|/port-id 01-02-03-04-06-{3}\n".format(interface_name, ovip_port, last_mac, last_mac)
	str+="/ieee802-dot1ab-lldp/lldp/port|name:{0}_{1}|dest-mac-address:01-02-03-04-06-{2}|/port-desc ${3}_${4}\n".format(interface_name, ovip_port, last_mac, interface_name, ovip_port)
	str+="/ieee802-dot1ab-lldp/lldp/port|name:{0}_{1}|dest-mac-address:01-02-03-04-06-{2}|/management-address-tx-port|address-subtype:ipv4|man-address:192.168.222.{3}|/tx-enable true\n".format(interface_name, ovip_port, last_mac, last_mac)
	str+="/ieee802-dot1ab-lldp/lldp/port|name:{0}_{1}|dest-mac-address:01-02-03-04-06-{2}|/management-address-tx-port|address-subtype:ipv4|man-address:192.168.222.{3}|/if-subtype 2\n".format(interface_name, ovip_port, last_mac, last_mac)
	str+="/ieee802-dot1ab-lldp/lldp/port|name:{0}_{1}|dest-mac-address:01-02-03-04-06-{2}|/management-address-tx-port|address-subtype:ipv4|man-address:192.168.222.{3}|/if-id 2\n".format(interface_name, ovip_port, last_mac, last_mac)
	str+="/ieee802-dot1ab-lldp/lldp/port|name:{0}_{1}|dest-mac-address:01-02-03-04-06-{2}|/management-address-tx-port|address-subtype:ipv6|man-address:fe80::eab9:1c3d:7ca5:50{3}|/tx-enable true\n".format(interface_name, ovip_port, last_mac, last_mac)
	str+="/ieee802-dot1ab-lldp/lldp/port|name:{0}_{1}|dest-mac-address:01-02-03-04-06-{2}|/management-address-tx-port|address-subtype:ipv6|man-address:fe80::eab9:1c3d:7ca5:50{3}|/if-subtype 2\n".format(interface_name, ovip_port, last_mac, last_mac)
	str+="/ieee802-dot1ab-lldp/lldp/port|name:{0}_{1}|dest-mac-address:01-02-03-04-06-{2}|/management-address-tx-port|address-subtype:ipv6|man-address:fe80::eab9:1c3d:7ca5:50{3}|/if-id 2\n".format(interface_name, ovip_port, last_mac, last_mac)

	f=open(file_name, "w")
	f.write(str)
	f.close

def start_uniconf(msg_fast_tx, msg_tx_hold, tx_interval, reinit_delay, tx_credit_max, tx_fast_init):
	create_ietf_config_file (INTERFACE0, "true", "up", 0)
	create_ietf_config_file (INTERFACE1, "true", "up", 1)
	create_lldp_config_file(INTERFACE0, 0, msg_fast_tx, msg_tx_hold, tx_interval, reinit_delay, tx_credit_max, tx_fast_init)
	create_lldp_config_file(INTERFACE1, 1, msg_fast_tx, msg_tx_hold, tx_interval, reinit_delay, tx_credit_max, tx_fast_init)
	global uniconf1_pid
	global uniconf2_pid
	uniconf1_pid = subprocess.Popen(["uniconf", "-p", DB0, "-c", "ietf-cbeth0.conf", "-c", "lldp-cbeth0.conf"])
	time.sleep(1)
	uniconf2_pid = subprocess.Popen(["uniconf", "-p", DB1, "-c", "ietf-cbeth1.conf", "-c", "lldp-cbeth1.conf"])
	time.sleep(1)
	print("uniconf processes started [{0}-{1}]".format(uniconf1_pid.pid, uniconf2_pid.pid))

def stop_uniconf():
	global uniconf1_pid
	global uniconf2_pid
	print("Stop uniconf pids [{0}-{1}]".format(uniconf1_pid.pid, uniconf2_pid.pid))
	if uniconf1_pid.pid > 0:
		os.kill(uniconf1_pid.pid, signal.SIGTERM) #or signal.SIGKILL 
		print("killed uniconf: ", uniconf1_pid.pid)
	if uniconf2_pid.pid > 0:
		os.kill(uniconf2_pid.pid, signal.SIGTERM) #or signal.SIGKILL 
		print("killed uniconf: ", uniconf2_pid.pid)

	shutil.rmtree(DB0, ignore_errors=True)
	shutil.rmtree(DB1, ignore_errors=True)

def start_lldpd0():
	global lldp_pid0
	lldp_pid0 = subprocess.Popen(["../lldpd", "-p", "lldp_db_cbeth0"])
	print("Started lldpd0 ", lldp_pid0.pid)


def start_lldpd1():
	global lldp_pid1
	lldp_pid1 = subprocess.Popen(["../lldpd", "-p", "lldp_db_cbeth1"])
	print("Started lldpd1 ", lldp_pid1.pid)

def stop_lldpd0():
	global lldp_pid0
	if lldp_pid0.pid > 0:
		os.kill(lldp_pid0.pid, signal.SIGTERM) #or signal.SIGKILL
		print("killed lldp_pid0: ", lldp_pid0.pid)

def stop_lldpd1():
	global lldp_pid1
	if lldp_pid1.pid > 0:
		os.kill(lldp_pid1.pid, signal.SIGTERM) #or signal.SIGKILL
		print("killed lldp_pid1: ", lldp_pid1.pid)

### Normal case we will start both
def start_lldpd():
	start_lldpd0()
	time.sleep(1)
	start_lldpd1()

def stop_lldpd():
	stop_lldpd0()
	stop_lldpd1()

def capture_lldpclient0_output():
	result = subprocess.run(['../lldpclient', '-P', DB0, '--show-neighbors', INF_NAME0], stdout=subprocess.PIPE)
	return str(result)

def string_contain(msg, search):
	_logger.info("Searching {0}".format(search))
	if (str.__contains__(msg, search)):
		_logger.info("{} found".format(search))
		raise PassExecution("{} found".format(search))
	else:
		raise AssertionError("{} not found".format(search))

def tc_001():
	_logger.info("Cover test spec LLDPD_01, LLDPD_02 ")
	start_uniconf(1, 2 ,5, 2, 4, 2)
	start_lldpd()
	time.sleep(5)
	stop_lldpd()
	stop_uniconf()
