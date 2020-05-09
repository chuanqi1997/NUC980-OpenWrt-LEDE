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
import modbus_tk.defines as cst
import traceback
'''
config device
	opton  name 'MODBUS'
	option enabled '1'
	#rtu tcp
	option protocol 'tcp'
	option device_detail '/usr/device/mb_tcp.xml'
	option config_md5 'MD5_VALUE'
'''
log.basicConfig(level=log.DEBUG, format=' %(asctime)s - %(levelname)s- %(message)s')
#log.basicConfig(level=log.DEBUG, format='%(levelname)s- %(message)s')
try:
	# PORT = '/dev/ttyUSB0'
	#PORT = sys.argv[1]
	ptl = sys.argv[1]
	config_xml = sys.argv[2]
	log.debug('Hello World!')
	log.debug('ptl:%s', ptl)
	log.debug("config_xml:%s", config_xml)
except Exception as e:
	ptl = 'rtu'
	PORT = '/dev/pts/3'
	config_xml = "./ptl_relay.xml"
	log.debug('not args')


# save the clooect point to relay point detail
collect_relay_dic = {}
# save the relay dev id to dev class
relay_id_class_dic = {}
relay_collect_dic = {}
def parse_config_xml(config):
	log.debug('Start of parse_config_xml')
	mb_relay_l = []
	iec101_rly_l = []
	global collect_relay_dic
	global relay_id_class_dic
	global relay_collect_dic
	try:
		tree = ET.parse(config)
		root = tree.getroot()
	except Exception:
		log.debug("Error:cannot parse file:",config)
		sys.exit(1)
	log.debug('root tar:' + root.tag)
	#找到root节点下的所有relay_table节点
	mb_rly_list = {}
	try:

		for dev in root.findall('relay_dev'):
			ptl = dev.get('ptl')
			dev_id_r = int(dev.get("dev_id_r"))
			if( "modbus" in ptl):
				dev_class = dev_mb.ModbusSlave(
				name=dev.get('name'),
				port=int(dev.get("port")),
				baudrate=dev.get('bandrate'),
				bytesize=int(dev.get('bytesize')),
				parity=dev.get("parity"), 
				ptl=ptl,
				ip=dev.get("ip"),
				slave_id=int(dev.get("slave_id")))

				#print("mb_dev_l.measure_table.count ")
			elif (ptl == "IEC-101"):
				dev_class = dev_mod.DeviceModel()
			elif (ptl == "DL645-2007"):
				pass

			for measure_table in dev.findall('measure_table'):
				count = int(measure_table.get("count"))
				reg_start = int(measure_table.get("reg_start"))
				block_id = int(measure_table.get("block_id"))
				# add a measure table block detail
				dp = dev_mod.DevPoint(reg_start=reg_start, count=count, block_id=block_id)
				'''
				reg_start is the report register start address
				count is the register amount
				block_id is this measure_table's id
				'''
				for measure_point in measure_table.findall('measure_point'):
					# print("measure_point:", measure_point.get("desc_name"))
					dev_id_s = int(measure_point.get("dev_id_s"))
					name = measure_point.get("desc_name")
					report_id_r = int(measure_point.get("report_id_r"))
					point_id_s = int(measure_point.get("point_id_s"))
					index_id_s = int(measure_point.get("index_id_s"))
					# this point from dev which dev_id is this
					#point_detail = dev_mod.PointDetail(name=name, dev_id=dev_id_s, report_id=report_id, point_id=point_id)
					# add a point detail hash ,the key is the report id (report address), value is the point detail
					dph = dev_mod.DevicePointHash(dev_id=dev_id_s, index=index_id_s, point_id=point_id_s, pointType=dev_mod.MEASURE_TYPE)
					rph = dev_mod.RelayPointHash(dev_id_r=dev_id_r, block_id=block_id,
												 report_id_r=report_id_r, pointType=dev_mod.MEASURE_TYPE)
					#add to hash table
					collect_relay_dic[dph.GetHashKey()] = rph.GetHashKey()
					#add to hash relay collect dic
					relay_collect_dic[rph.GetHashKey()] = dph.GetHashKey()

					log.debug("dph:%s", dph.GetHashKey())
					log.debug("rph:%s", rph.GetHashKey())
					#dp.point_detail[measure_point.get("report_id")] = point_detail
				# add measure table detail
				dev_class.Measure_Table_Add(dp)
			for control_table in dev.findall('control_table'):
				count = int(control_table.get("count"))
				reg_start = int(control_table.get("reg_start"))
				block_id = int(control_table.get("block_id"))
				dp = dev_mod.DevPoint(reg_start=reg_start, count=count, block_id=block_id)
				for control_point in control_table.findall('control_point'):
					dev_id_s = int(control_point.get("dev_id_s"))
					report_id_r = int(control_point.get("report_id_r"))
					point_id_s = int(control_point.get("point_id_s"))
					index_id_s = int(control_point.get("index_id_s"))
					dph = dev_mod.DevicePointHash(dev_id=dev_id_s, index=index_id_s, point_id=point_id_s,
												  pointType=dev_mod.CONTROL_TYPE)
					rph = dev_mod.RelayPointHash(dev_id_r=dev_id_r, block_id=block_id,
												 report_id_r=report_id_r, pointType=dev_mod.CONTROL_TYPE)
					collect_relay_dic[dph.GetHashKey()] = rph.GetHashKey()

					log.debug("dph:%s", dph.GetHashKey())
					log.debug("rph:%s", rph.GetHashKey())
				dev_class.Control_Table_Add(dp)

			relay_id_class_dic[dev_id_r] = dev_class
			if ("modbus" in ptl):
				mb_relay_l.append(dev_class)  # add the modbu device to the dev list, if
			elif (ptl == "IEC-101"):
				iec101_rly_l.append(iec101_dev_s)
	except Exception:
		log.debug("Error:cannot parse file:", config)
		sys.exit(1)
	return mb_relay_l, iec101_rly_l

def mqtt_init():
	client = mqtt.Client()
	#client.username_pw_set("admin", "password")  # 必须设置，否则会返回「Connected with result code 4」
	client.on_connect = on_connect
	#client.on_message = on_message
	#HOST = "127.0.0.1"
	hostname = socket.gethostname()
	log.debug(hostname)
	client.connect(host=hostname)
	# client.loop_forever()
	#user = input("请输入名称:")
	#client.user_data_set(user)
	client.loop_start()
	return client
	#client.loop_forever()

def modbus_reply(mb_slave_l, mqtt_client, run):


	global  relay_collect_dic

	def mb_on_message(client, userdata, msg):
		#log.debug("mb_on_message")
		log.debug("mb_on_message:%s", msg.topic + ":" + msg.payload.decode())
		msg_json_dic = json.loads(msg.payload.decode())

		try:
			dev_id = int(msg_json_dic["dev_id"])
			cmd = msg_json_dic["cmd"]
			index = int(msg_json_dic["index"])
			pointType = int(msg_json_dic["pointType"])
			count = int(msg_json_dic["count"])
			value = msg_json_dic["value"]
			reg_start = int(msg_json_dic["reg_start"])

			global collect_relay_dic
			global relay_id_class_dic

			for i, point_id in enumerate(range(reg_start, reg_start+count)):
				dph = dev_mod.DevicePointHash(dev_id=dev_id, index=index, point_id=point_id, pointType=pointType)
				dev_hash_key = dph.GetHashKey()
				try:
					if dev_hash_key in collect_relay_dic:
						log.debug("has key!")

						#{"dev_id_r": 1, "block_id": "1", "pointType": 1, "report_id_r": 0}
						relay_json_val = collect_relay_dic[dev_hash_key]
						relay_hash_val = json.loads(relay_json_val)
						dev_id_r = int(relay_hash_val["dev_id_r"])
						block_id = int(relay_hash_val["block_id"])
						pointType_r = int(relay_hash_val["pointType"])
						report_id_r = int(relay_hash_val["report_id_r"])

						dev_cass = relay_id_class_dic[dev_id_r]
						dev_cass.Set_Values(block_id=block_id,
											address=report_id_r, values=value[i])
						#log.debug("block_id %d address %d values=%d", block_id, report_id_r, value[i])
						log.debug("dev_id_r:%d block_id:%d, pointType_r:%d report_id_r:%d",
								  dev_id_r, block_id, pointType_r,report_id_r)
					else:
						#log.debug("not has this key the msg hash key is %s", dev_hash_key)
						pass
						#for index, key in enumerate(collect_relay_dic):
						#	log.debug("collect_relay_dic hash %d: val:%s", index, hash(key))
				except Exception as e:
					traceback.print_exc()
		except Exception as e:
			traceback.print_exc()
		'''
		from the dev report message we cae parse some device detail information
		such as dev_id , dev_state , report function , the index of the device collect table
		we should build a relation between the message report message and the report date table
		'''
		# first we should get the dev point (using the dev_id )
		# then we should
	for dev in mb_slave_l:
		log.debug(" modbus_reply name :%s ,slave_id:%d", dev.name, dev.slave_id)
		dev.Link_Init()
		dev.Slave_Init()
		for m_point in dev.measure_table:
			dev.Slave_Add_Block(block_id=m_point.block_id,
								block_type=cst.READ_INPUT_REGISTERS,
								address=m_point.reg_start,
								size=m_point.count)
		for index, c_point in enumerate(dev.control_table):
			dev.Slave_Add_Block(block_id=c_point.block_id,
								block_type=cst.READ_COILS,
								address=c_point.reg_start,
								size=c_point.count)
			dev.do_data_cache.append( [0]*c_point.count)
			dev.do_data.append( [0]*c_point.count)
			#dev.Set_Values()
			#dev.Set_Values(slave_id=dev.slave_id, block_id=m_point.block_id,address= m_point.reg_start, values=range(m_point.count))
	mqtt_client.on_message = mb_on_message
	while run:
		for dev_index, dev in enumerate(mb_slave_l):
			for point_index, c_point in enumerate(dev.control_table):

				dev.do_data_cache[point_index] = dev.Get_Values(
				block_id=c_point.block_id,
				address=c_point.reg_start,
				length=c_point.count)
				if dev.do_data[point_index] != dev.do_data_cache[point_index]:
					log.debug("do_val changed !")

					for i in range(0, c_point.count):
						a = dev.do_data[point_index]
						b =  dev.do_data_cache[point_index]
						if(a[i] != b[i]):
							log.debug("find addr index:%d", i)
							dev_id = dev.dev_id
							block_id = c_point.block_id

							rph = dev_mod.RelayPointHash(dev_id_r=dev_id, block_id=block_id,pointType=CONTROL_TYPE,
														 report_id_r=address+i)

							relay_hash_key = rph.GetHashKey()
							log.debug("%s", relay_hash_key)
							try:
								if relay_hash_key in relay_collect_dic:
									log.debug("find control dev")

									dev_id = int(relay_hash_key["dev_id"])
									block_id = int(relay_hash_key["block_id"])
									pointType_r = int(relay_hash_key["pointType"])
									report_id_r = int(relay_hash_key["report_id_r"])

									payload = json.dumps({
										"dev_id": dev_id,
										}
									)

									mqtt_client.publish("chat", payload)

							except Exception as e:
									traceback.print_exc()
				dev.do_data[point_index] = dev.do_data_cache[point_index]

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
			#def modbus_reply(mb_slave_l, mqtt_client, run):
			self.func(self.mb_dev_l, self.mqtt_client, self.Run)
		#modbus_collect(self.threadName, self.mb_dev_l)
	def stop(self):
		self.Run = False

def on_connect(client, userdata, flags, rc):
    log.debug("Connected with result code %s", str(rc))
    client.subscribe("chat")
    #client.publish("chat", json.dumps({"user": "user", "say": "Hello,anyone!"}))

def on_message(client, userdata, msg):
    print(msg.topic+":"+str(msg.payload.decode()))
    print(msg.topic+":"+msg.payload.decode())
    payload = json.loads(msg.payload.decode())
    print(payload.get("user")+":"+payload.get("say"))

def main():
	log.debug('this message is from main function')
	mb_relay_l, iec101_rly_l = parse_config_xml(config_xml)
	log.debug("  parse final!")
	client = mqtt_init()
	# 创建两个线程
	try:
		#_thread.start_new_thread(modbus_collect, ("modbus-thread", mb_dev_l))
		#_thread.start_new_thread(print_time, ("Thread-2", 4))
		#modbus_thread = threading.Thread(target=modbus_collect,
		#								 args=("modbus-thread", mb_dev_l))
		#modbus_thread.start()

		modbus_replay = ModBusThread(threadName="modbus-replay",
									 func=modbus_reply,
									 mb_dev_l=mb_relay_l,
									 client=client)
		modbus_replay.start()

		#modbus_thread.join()
		#modbus_thread.stop()
	except:
		print("Error: 无法启动线程")
	#while True:
	#	time.sleep(5)
	return
if __name__ == '__main__':
	main()