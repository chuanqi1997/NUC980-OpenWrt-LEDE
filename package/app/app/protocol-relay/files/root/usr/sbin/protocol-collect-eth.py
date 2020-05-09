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
			if( ptl== "MODBUS-RTU"):
				mb_dev = dev_mb.DeviceModbus(name = dev.get('name'),
											 port = PORT,
											 baudrate = dev.get('bandrate'),
											 bytesize = int(dev.get('bytesize')),
											parity = dev.get("parity"),
											 slave = int(dev.get("addr")))

				#print("MODBUS-RTU", dev.get('name'))

				for measure_table in dev.findall('measure_table'):
					count = int(measure_table.get("count"))
					reg_start = int(measure_table.get("reg_start"))
					dp = dev_mod.DevPoint(reg_start=reg_start, count=count)
					#print("measure_table")
					for measure_point in measure_table.findall('measure_point'):
						#print("measure_point:", measure_point.get("desc_name"))
						point_detail = dev_mod.PointDetail(
										name=measure_point.get("desc_name"),
										multiply_value=float(measure_point.get("multiply_value")),
										id=measure_point.get("id"))
						dp.point_detail[measure_point.get("id")] = point_detail
						#print(measure_point.get('desc_name'))

					#print("measure_table.append", dp.reg_start, dp.count)
					mb_dev.measure_table.append(dp)  # measure_table save the 04 func group
					#mb_dev.measure_table.append()
				for control_table in dev.findall('control_table'):
					#print('control_table count', control_table.get('count'))
					pass
					for control_point in control_table.findall('control_point'):
						#print(control_point.get('desc_name'))
						pass
				#print("mb_dev_l.measure_table.count ")
			elif (ptl == "IEC-101"):
				pass
			elif (ptl == "DL645-2007"):
				pass
			#global mb_dev_list
			mb_dev_l.append(mb_dev)  # add the modbu device to the dev list
	except Exception:
		log.debug("Error:cannot parse file:", config)
		sys.exit(1)
	return mb_dev_l, iec101_dev_l


def mqtt_init():
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
	mqtt_client.on_message =mb_on_message

	while run:
		for dev in mb_dev_l:
			#log.debug('dev_name:%s', dev.name)
			#log.debug('dev_addr:%s', dev.slave)
			dev.Link_Init()
			log.debug("measure_table len:%d", len(dev.measure_table))
			for m_list in dev.measure_table:
				res = dev.Read_Input_Registers(dev.slave, m_list.reg_start, m_list.count)
				if res != None:
					log.debug("res")
				else:
					log.debug("%s no ack!", dev.name)
					mqtt_client.publish("chat", json.dumps({"dev": dev.name, "say": "No Ack!"}))
			dev.Link_Relase()
		time.sleep(0.1)
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