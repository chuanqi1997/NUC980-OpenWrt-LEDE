#!/usr/bin/python3

from http.server import HTTPServer, BaseHTTPRequestHandler
from urllib.parse import urlparse , parse_qs, parse_qsl
import paho.mqtt.client as mqtt
import socket
import json
import sys
import serial
import threading
import time
from  logging.handlers import RotatingFileHandler


log_file = "/var/log/ztk_service.log"
log_format = '%(asctime)s %(levelname)s %(message)s'


# import logging as log
# log.basicConfig(level=log.DEBUG, format=log_format)

import logging
from logging import handlers
log = logging.getLogger(__name__)
fh = handlers.RotatingFileHandler(filename=log_file, maxBytes=1024*8, backupCount=1)
formatter = logging.Formatter(log_format)
fh.setFormatter(formatter)
log.setLevel(logging.DEBUG)
log.addHandler(fh)




# log_formate = logging.Formatter(log_format)
# log_handler = RotatingFileHandler(log_file, mode='a', maxBytes=1024, backupCount=2)
# log_handler.setFormatter(log_formate)
# log_handler.setLevel(logging.DEBUG)
# level = logging.DEBUG
# log = logging.getLogger()
# log.setLevel(level)
# log.addHandler(log_handler)

# log.basicConfig(level=log.DEBUG, format=log_format, filename=log_file, filemode='a')







hostname = socket.gethostname()
loaclip = socket.gethostbyname(hostname)

try:
    # Options = "${HTTP_PORT} ${MQTT_HOST} ${MQTT_PORT} ${MQTT_TOPIC} ${DOOR_UART}"
    HTTP_PORT = int(sys.argv[1])
    MQTT_HOST = sys.argv[2]
    MQTT_PORT = int(sys.argv[3])
    MQTT_TOPIC = sys.argv[4]
    DOOR_UART = sys.argv[5]
except Exception as e:
    MQTT_HOST = "192.168.1.188"
    HTTP_PORT = 8080
    MQTT_PORT=1883
    MQTT_TOPIC = "/topic/ATTLOG"
    DOOR_UART = "/dev/ttyS1"

log.debug("hostname:%s loaclip:%s", str(hostname), str(loaclip))
log.debug("HTTP_PORT:%d MQTT_HOST:%s MQTT_PORT:%d MQTT_TOPIC:%s", HTTP_PORT, str(MQTT_HOST), MQTT_PORT, str(MQTT_TOPIC))

# init_data = {"success": bool(1), "server": "192.168.1.129", "port": 80}
init_data = {"success": bool(1), "server": loaclip, "port": HTTP_PORT}

host = (loaclip, HTTP_PORT)
# host = (hostname, HTTP_PORT)
# local_host = ("127.0.0.1", HTTP_PORT)


def on_connect(client, userdata, flags, rc):
    log.debug("MQTT Broker Connected Result:%s", str(rc))
    #client.subscribe(topic)
    # client.publish("chat", json.dumps({"user": "user", "say": "Hello,anyone!"}))

def on_message(client, userdata, msg):
    print(msg.topic + ":" + str(msg.payload.decode()))
    print(msg.topic + ":" + msg.payload.decode())
    payload = json.loads(msg.payload.decode())
    print(payload.get("user") + ":" + payload.get("say"))

def mqtt_init(host="192.168.1.188", port=1883):
    client = mqtt.Client()
    # client.username_pw_set("admin", "password")  # 必须设置，否则会返回「Connected with result code 4」
    client.on_connect = on_connect
    client.on_message = on_message
    try:
        # client.connect(host=host, port=port)
        client.connect_async(host=host, port=port, keepalive=30)
        client.reconnect_delay_set(min_delay=1, max_delay=3)
        client.loop_start()
    except Exception as e:
        log.warning(e)
    # client.loop_forever()
    return client

mqtt_client = mqtt_init(host=MQTT_HOST, port=MQTT_PORT)
msg_list_his=[]
log_list_his=[]

def mqtt_publish(payload, log_str):
    if mqtt_client.is_connected() == True:
        mqtt_client.publish(MQTT_TOPIC, payload)
        log.debug("MQTT PUB:%s", str(log_str))
    else:
        log.debug("MQTT Broker is Not Connected!")
        msg_list_his.append(payload)
        log_list_his.append(log_str)
        try:
            mqtt_client.reconnect_delay_set(min_delay=1, max_delay=3)
            mqtt_client.reconnect()
        except Exception as e:
            log.warning(e)

def mqtt_check_onile():
    if mqtt_client.is_connected() == False:
        log.debug("Try Reconnect MQTT Broker...")
        try:
            mqtt_client.reconnect_delay_set(min_delay=1, max_delay=3)
            mqtt_client.reconnect()
        except Exception as e:
            log.warning(e)
    elif mqtt_client.is_connected() == True:
        for payload in msg_list_his:
            mqtt_client.publish(MQTT_TOPIC, payload)
        msg_list_his.clear()

        for log_str in log_list_his:
            log.debug("PUB HIS MSG:%s", str(log_str))
        log_list_his.clear()

class DoorStateThread (threading.Thread):
    def __init__(self, uart):
        threading.Thread.__init__(self)
        self.uart = uart
        log.debug("DOOR_UART:%s", str(self.uart))
        self.com = serial.Serial(port=uart, timeout=0.2)
        if self.com.isOpen():
            self.com.close()
            self.com.open()

    def run(self):
        while True:
            if self.com.isOpen() == False:
                self.com.close()
                self.com.open()
                time.sleep(3)
            else:
                rec = self.com.readlines()
                if len(rec):
                    log.debug("UART:%s REC FRAME CNT:%d", str(self.uart), len(rec))
                    for val in rec:
                        try:
                            rec_json = json.loads(val)
                            log.debug("FRAME JSON:%s", str(rec_json))
                            # {"DoorState":"open"}
                            door_state = rec_json["DoorState"]
                            # {"MSGTYPE": "TGTM_Op", "TIME": "2020-01-22 10:22:34", "Value1": "open"}
                            Time = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
                            ListDict = {"MSGTYPE": "TGTM_Op",
                                        "TIME": Time,
                                        "Value1": door_state}
                            payload = json.dumps(ListDict, separators=(',', ':'))
                            mqtt_publish(payload=payload, log_str=str(ListDict))
                        except Exception as e:
                            log.warning(e)


class Resquest(BaseHTTPRequestHandler):

    def send_OK(self):
        OK = b'OK'
        self.protocol_version = 'HTTP/1.1'  # 设置协议版本
        self.send_response(200)
        self.send_header('Content-Type', 'text/plain')
        self.send_header('Content-Length', len(OK))
        self.send_header('Connection', 'close')
        self.send_header('Pragma', 'no-cache')
        self.send_header('Cache-Control', 'no-store')
        self.end_headers()
        self.wfile.write(OK)
        log.debug("SEND HTTP OK")

    def do_GET(self):
        #log.debug("self.path:%s", str(self.path))
        #log.debug(self.command)
        url = self.path #parse the url contion path
        #res = urlparse(url)
        #log.debug(res)
        # 1. 获取各主要参数
        uri = urlparse(url).path
        log.debug("ZTK GET URI %s", str(uri))
        # params = urlparse(url).params
        # query = urlparse(url).query
        if uri == "/UserServer/user":
            # 2. 获取详细查询信息
            query = parse_qs(urlparse(url).query)
            self.SN = query['sn'][0]
            action = query['action'][0]
            log.debug("ZTK Device Connected SN:%s", str(self.SN))
            if action == 'getserver':
                # log.debug("getserver")
                self.protocol_version = 'HTTP/1.1'  # 设置协议版本
                self.send_response(200)
                """
                HTTP / 1.1 200 OK
                Server: nginx
                Date: Mon, 17
                Feb
                2020
                0
                8: 22:27
                GMT
                Content - Length: 58
                Connection: close
                {"success": true, "server": "192.168.1.129", "port": 9999}
                """
                #{"success": true, "server": "192.168.1.129", "port": 9999}
                #'{"success": "true", "server": "192.168.1.129", "port": 80}'
                msg = json.dumps(init_data).encode()
                self.send_header('Content-Length', len(msg))
                self.send_header('Connection', 'close')
                self.end_headers()
                log.debug("Init ZTK Dev SEND MSG:%s", str(init_data))
                # log.debug("SEND:")
                # log.debug(msg)
                # log.debug("MSG LEN: %d", len(msg))
                self.wfile.write(msg)
                mqtt_check_onile()# check the mqtt broker is online

        elif uri == "/iclock/getrequest":
            self.send_OK()
            mqtt_check_onile()  # check the mqtt broker is online

        elif uri == "/iclock/cdata":
            self.send_OK()
            # OK = b'OK'
            # self.protocol_version = 'HTTP/1.1'  # 设置协议版本
            # self.send_response(200)
            # self.send_header('Content-Type', 'text/plain')
            # self.send_header('Content-Length', len(OK))
            # self.send_header('Connection', 'close')
            # self.send_header('Pragma', 'no-cache')
            # self.send_header('Cache-Control', 'no-store')
            # self.end_headers()
            # log.debug(uri, "send msg len:", len(OK))
            # self.wfile.write(OK)
            mqtt_check_onile()  # check the mqtt broker is online

    def do_POST(self):
        #post_data = json.loads(self.rfile.read(length))
        #log.debug("self.path %s", self.path)
        url = self.path #parse the url contion path
        # 1. 获取各主要参数
        uri = urlparse(url).path
        log.debug("ZTK POST URI:%s", uri)
        if uri == "/iclock/cdata":
            # 2. 获取详细查询信息
            length = int(self.headers['Content-Length'])
            msg = self.rfile.read(length)
            query = parse_qs(urlparse(url).query)
            log.debug(query)
            try:
                table = query['table'][0]
                SN = query['SN'][0]

                #Stamp = query['Stamp'][0]
                #log.debug("table: %s Stamp: %s", str(table), str(Stamp))


                if table == "ATTLOG":#考勤记录
                    # log.debug("ATTLOG:%s", str(msg).strip('\n'))
                    strlist = str(msg, encoding="utf8")  # bytes to str
                    msg_data = strlist.split('\t')
                    # log.debug("msg_data %s:", msg_data)
                    Pin = msg_data[0]
                    Time = msg_data[1]
                    Status = msg_data[2]
                    Verify = msg_data[3]
                    Workcode = msg_data[4]
                    log.debug("ATTLOG Pin:%s Time:%s Status:%s Verify:%s", str(Pin), str(Time), str(Status), str(Verify))
                    #val['20156107', '2020-02-22 23:56:16', '255', '1', '', '0', '0', '8\n']:
                    #for value in strlist: # 循环输出列表值
                    #msg: b'20156107\t2020-02-22 23:10:05\t255\t1\t\t0\t0\t2\n'
                    # 请求实体： ${DataRecord}， 考勤记录数据， 数据格式如下
                    # ${Pin}${HT}${Time}${HT}${Status}${HT}${Verify}${HT}${Workcode}${HT}${Reserved}${HT}${Reser
                    # ved}
                    # json.dumps(ListDict, separators=(',', ':'))
                    # 2.
                    # 上传考勤记录，格式如下（中间没有\n\r之类的字符）：
                    # {"MSGTYPE": "ATTLOG", "SN": "0316144680030", "USERID": "1988993", "TIME": "2019-07-10 10:10:10",
                    # "TYPE": "1"}
                    #
                    # 下面对每一个属性进行解释
                    # MSGTYPE：上送的报文格式，目前只有一种信息上送是ATTLOG（上传考勤记录），具体内容见附录一；
                    # SN：每一个考勤机的唯一编码，一般是一个13字符的字符串；
                    # USERID：录入到考勤机的用户ID；
                    # TIME：考勤的时间；
                    # TYPE：通过考勤机考勤的方式，具体编号见附录二。

                    ListDict = {"MSGTYPE": "ATTLOG", "SN": SN, "USERID": Pin, "TIME": Time, "TYPE": Verify}
                    payload = json.dumps(ListDict, separators=(',', ':'))
                    mqtt_publish(payload=payload, log_str=str(ListDict))


                elif table == "OPERLOG":
                    # log.debug("OPERLOG:%s", str(msg).strip('\n'))
                    # DEBUG - {'SN': ['AB3N191860207'], 'table': ['OPERLOG'], 'OpStamp': ['9999']}
                    # DEBUG - OPERLOG: b'OPLOG 4\t0\t2020-02-23 00:04:08\t20156107\t0\t0\t0\n'
                    strlist = str(msg, encoding="utf8")  # bytes to str
                    msg_data = strlist.split('\t')
                    # log.debug("msg_data %s:", msg_data)
                    # OPLOG${SP}${OpType}${HT}
                    # ${Operator}${HT}
                    # ${OpTime}${HT}
                    # ${OpWho}${HT}
                    # ${Value1}${HT}
                    # ${Value2}${HT}
                    # ${Value3}
                    #  OPERLOG: b'OPLOG 4\t0\t2020-02-23 00:08:58\t20156107\t0\t0\t0\n'
                    #  DEBUG - msg_data['OPLOG 4', '0', '2020-02-23 00:08:58', '20156107', '0', '0', '0\n']:
                    # OpType
                    OpType = msg_data[0].split(maxsplit=1)[1]
                    Operator = msg_data[1]
                    Time = msg_data[2]
                    Pin = msg_data[3]
                    Value1 = msg_data[4]
                    Value2 = msg_data[5]
                    Value3 = msg_data[5]
                    log.debug("OPERLOG OpType:%s Operator:%s Time:%s Pin:%s Value1:%s", str(OpType), str(Operator), str(Time), str(Pin), str(Value1))

                    # 3.
                    # 上传操作记录
                    # {"MSGTYPE": "OPLOG", "SN": "0316144680030", "OpType": "3", "Operator": "0",
                    #  "TIME": "2019-07-20 10:22:34", "OpWho": "54", "Value1": "1", "Value2": "0", "Value3": "0"}
                    # "
                    # OpType：值见附录三，如果是刷脸和刷指纹这个值就是为3；
                    # Operator：默认0；
                    # TIME：操作时间；
                    # OpWho：值见附录四，何种操作；
                    # Value1：
                    # Value2：默认0；
                    # Value3：默认0。

                    ListDict = {"MSGTYPE": "OPLOG",
                                "SN": SN,
                                "OpType": OpType,
                                "Operator": Operator,
                                "TIME": Time,
                                "OpWho": Pin,
                                "Value1": Value1, "Value2": "0", "Value3": "0"}
                    payload = json.dumps(ListDict, separators=(',', ':'))
                    mqtt_publish(payload=payload, log_str=str(ListDict))
            except Exception as e:
                log.warning(e)
        # params = urlparse(url).params
        # query = urlparse(url).query
        self.send_OK()
        mqtt_check_onile()  # check the mqtt broker is online
        # global mqtt_client
        # if mqtt_client.is_connected() == True:
        #     self.send_OK()
        # else:
        #     self.send_error(code=401)

if __name__ == '__main__':
    doorThread = DoorStateThread(uart=DOOR_UART)
    doorThread.start()
    server = HTTPServer(host, Resquest)
    log.debug("Starting ZTK HTTP Server....")
    server.serve_forever()

# 定义
# 文档中引用定义使用格式为： ${ServerIP}
# ●ServerIP： 服务器IP地址
# ●ServerPort： 服务器端口
# ●XXX： 未知值7
# ●Value1\Value2\Value3……\Valuen： 值1\值2\值3……值n
# ●Required： 必须存在
# ●Optional： 可选
# ●SerialNumber： 系列号(可以为字母、 数字、 字母+数字组合)
# ●NUL： null（\0）
# ●SP： 空格
# ●LF： 换行符（\n）
# ●HT： 制表符（\t）
# ●DataRecord： 数据记录
# ●CmdRecord： 命令记录
# ●CmdID： 命令编号
# ●CmdDesc： 命令描述
# ●Pin： 工号
# ●Time： 考勤时间
# ●Status： 考勤状态
# ●Verify： 验证方式
# ●Workcode： workcode编码
# ●Reserved： 预留字段
# ●OpType： 操作类型
# ●OpWho： 操作者
# ●OpTime： 操作时间
# ●BinaryData： 二进制数据流
# ●TableName： 数据表名
# ●SystemCmd： 系统命令
# ●Key： 键
# ●Value： 值
# ●FilePath： 文件路径
# ●URL： 资源位置

