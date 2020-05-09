#!/usr/bin/python3
#-*- coding:utf-8 -*-
import logging as log
import sqlite3
import sys
import xml.etree.ElementTree as ET
from dev_mod import dev_modbus as dev_mb
from dev_mod import dev_model as dev_mod
import time
import paho.mqtt.client as mqtt
import threading
import json
import socket

try:
	# PORT = '/dev/ttyUSB0'
	PORT = sys.argv[1]
	config_xml = sys.argv[2]
	print('Hello World!')
	print('PORT:', PORT)
	print("config_xml", config_xml)
except Exception as e:
	PORT = '/dev/pts/3'
	config_xml = "./MODBUS_HJ.xml"
	print('not args')

def parse_config_xml(config):
	#log.debug('Start of parse_config_xml')
	mb_dev_l = []
	iec101_dev_l = []
	try:
		tree = ET.parse(config)
		root = tree.getroot()
	except Exception:
		log.debug("Error:cannot parse file:",config)
		sys.exit(1)
	log.debug('root tar:' + root.tag)
	#找到root节点下的所有device节点
	mb_dev_list = {}
	try:

		for dev in root.findall('device'):
			#print(device.get('name'), device.get('addr'),device.get('bandrate'))
			#MODBUS-RTU
			ptl = dev.get('ptl')
			if( ptl== "modbus-rtu"):
				mb_dev = dev_mb.DeviceModbus(name=dev.get('name'),
											port = PORT,
											baudrate = dev.get('bandrate'),
											bytesize = int(dev.get('bytesize')),
											parity = dev.get("parity"),
											dev_id = int(dev.get("dev_id")),
											slave = int(dev.get("addr")))
				#print("MODBUS-RTU", dev.get('name'))
			elif (ptl == "IEC-101"):
				pass

			elif (ptl == "DL645-2007"):
				pass

			for measure_table in dev.findall('measure_table'):
				count = int(measure_table.get("count"))
				reg_start = int(measure_table.get("reg_start"))
				dp = dev_mod.DevPoint(reg_start=reg_start, count=count)
				# print("measure_table")
				for measure_point in measure_table.findall('measure_point'):
					# print("measure_point:", measure_point.get("desc_name"))
					point_detail = dev_mod.PointDetail(
						name=measure_point.get("desc_name"),
						multiply_value=float(measure_point.get("multiply_value")),
						point_id=measure_point.get("point_id"))
					dp.point_detail[measure_point.get("point_id")] = point_detail
				# print(measure_point.get('desc_name'))

				# print("measure_table.append", dp.reg_start, dp.count)
				#mb_dev.measure_table.append(dp)  # measure_table save the 04 func group
				mb_dev.Measure_Table_Add(dp)
			# mb_dev.measure_table.append()
			for control_table in dev.findall('control_table'):
				# print('control_table count', control_table.get('count'))
				pass
				for control_point in control_table.findall('control_point'):
					# print(control_point.get('desc_name'))
					pass
			# print("mb_dev_l.measure_table.count ")
			#global mb_dev_list
			mb_dev_l.append(mb_dev)  # add the modbu device to the dev list
	except Exception:
		log.debug("Error:cannot parse file:", config)
		sys.exit(1)
	return mb_dev_l, iec101_dev_l

class ModBusThread(threading.Thread):
	def __init__(self, func, threadName = "modbus_thread", mb_dev_l = [], client=None):
		super().__init__()
		self.threadName = threadName
		self.mb_dev_l = mb_dev_l
		self.func = func
		self.mqtt_client = client
		self.Run = True
	def run(self):
		while self.Run:
			self.func(self.mb_dev_l, self.mqtt_client, self.Run)
		#modbus_collect(self.threadName, self.mb_dev_l)
	def stop(self):
		self.Run = False

def mqtt_init():
	"""mqtt init"""
	client = mqtt.Client()
	#client.username_pw_set("admin", "password")  # 必须设置，否则会返回「Connected with result code 4」
	client.on_connect = on_connect
	client.on_message = on_message
	#HOST = "127.0.0.1"
	hostname = socket.gethostname()
	print(hostname)
	client.connect(host=hostname)
	# client.loop_forever()
	#user = input("请输入名称:")
	#client.user_data_set(user)
	client.loop_start()
	return client
	#client.loop_forever()

def modbus_collect(mb_dev_l, mqtt_client, run):

	def mb_on_message(client, userdata, msg):
		log.debug("mb_on_message")
		#print(msg.topic + ":" + str(msg.payload.decode()))
		log.debug("%s", msg.topic + ":" + msg.payload.decode())
		#payload = json.loads(msg.payload.decode())
		#print(payload.get("user") + ":" + payload.get("say"))

	mqtt_client.on_message = mb_on_message

	while run:
		for dev in mb_dev_l:
			log.debug('dev_name:%s', dev.name)
			#log.debug('dev_addr:%s', dev.slave)
			dev.Link_Init()
			#log.debug("measure_table len:%d", len(dev.measure_table))
			#for m_list in dev.measure_table:
			for index, m_list in enumerate(dev.measure_table):
				log.debug("index:%d", index)
				res = dev.Read_Input_Registers(dev.slave, m_list.reg_start, m_list.count)
				if res != None:
					log.debug("res ok")
					'''
					        Key = json.dumps(
					     {"dev_id": self.dev_id,
                          "index": self.index,
                          "pointType":self.pointType,
                          "point_id":self.point_id
                          })
					'''
					payload =  json.dumps({"dev_id": int(dev.dev_id[0]),
											"index": index,
											"cmd": dev_mod.CMD_REPORT_NORMAL,
											"pointType": dev_mod.MEASURE_TYPE,
											"reg_start": m_list.reg_start,
											"count": m_list.count,
											"value": res})
					mqtt_client.publish("chat", payload)

				else:
					log.debug("%s no ack!", dev.name)
					payload =  json.dumps({"dev_id": int(dev.dev_id[0]),
											"cmd": dev_mod.CMD_DEVICE_NO_ACK})
					mqtt_client.publish("chat", payload)
			dev.Link_Relase()
		time.sleep(3)



def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("chat")
    #client.publish("chat", json.dumps({"user": "user", "say": "Hello,anyone!"}))

def on_message(client, userdata, msg):
    print(msg.topic+":"+str(msg.payload.decode()))
    print(msg.topic+":"+msg.payload.decode())
    payload = json.loads(msg.payload.decode())
    print(payload.get("user")+":"+payload.get("say"))

def main():
	log.basicConfig(level=log.DEBUG, format=' %(asctime)s - %(levelname)s- %(message)s')
	print('this message is from main function')
	mb_dev_l, iec101_dev_l = parse_config_xml(config_xml)
	print("  parse final!")
	client = mqtt_init()
	# 创建两个线程
	try:
		#_thread.start_new_thread(modbus_collect, ("modbus-thread", mb_dev_l))
		#_thread.start_new_thread(print_time, ("Thread-2", 4))
		#modbus_thread = threading.Thread(target=modbus_collect,
		#								 args=("modbus-thread", mb_dev_l))
		#modbus_thread.start()

		modbus_thread = ModBusThread(threadName="modbus-thread",
									 func=modbus_collect,
									 mb_dev_l=mb_dev_l,
									 client=client)
		modbus_thread.start()
		#modbus_thread.join()
		#modbus_thread.stop()
	except:
		print("Error: 无法启动线程")

	#while True:
	#	time.sleep(1)

	return
if __name__ == '__main__':
	main()
	'''
	def Modbus_Master_Test():
		logger = modbus_tk.utils.create_logger("console")
		try:
			#Connect to the slave
			master = modbus_rtu.RtuMaster(
				serial.Serial(port=PORT, baudrate=9600, bytesize=8, parity='N', stopbits=1, xonxoff=0)
			)

			master.set_timeout(5.0)
			master.set_verbose(True)
			logger.info("connected")

			while True:
				#send some queries
				logger.info(master.execute(1, cst.READ_COILS, 0, 10))

				logger.info(master.execute(1, cst.READ_DISCRETE_INPUTS, 0, 8))

				logger.info(master.execute(1, cst.READ_INPUT_REGISTERS, 0, 3))
				logger.info(master.execute(1, cst.READ_HOLDING_REGISTERS, 0, 12))
				logger.info(master.execute(1, cst.WRITE_SINGLE_COIL, 7, output_value=1))
				logger.info(master.execute(1, cst.WRITE_SINGLE_REGISTER, 0, output_value=54))
				logger.info(master.execute(1, cst.WRITE_MULTIPLE_COILS, 0, output_value=[1, 1, 0, 1, 1, 0, 1, 1]))
				#logger.info(master.execute(1, cst.WRITE_MULTIPLE_REGISTERS, 0, output_value=xrange(12)))
		except modbus_tk.modbus.ModbusError as exc:
			logger.error("%s- Code=%d", exc, exc.get_exception_code())
		time.sleep(2)
	'''
