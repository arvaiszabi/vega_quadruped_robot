import socket
import cv2
import struct
import pickle
import netifaces as ni
import threading
import RPi.GPIO as GPIO
import serial
import time
import os

# 'Konstansok'
ARDUINO_VIN = 13 # GPIO4 - Arduino tápellátás FET meghajtó
MOTOR_VIN = 7  # GPIO27 - Szervómotorok tápellátása FET meghajtó
DIRECTIONS = ["forw\n", "back\n", "left\n", "right\n", "stop\n"]

# Raspberry pin mapping beállítása
GPIO.setmode(GPIO.BOARD)
GPIO.setwarnings(False)

# Kimenetek konfigurálása
GPIO.setup(ARDUINO_VIN, GPIO.OUT)
GPIO.output(ARDUINO_VIN, GPIO.LOW)
time.sleep(0.5)
GPIO.setup(MOTOR_VIN, GPIO.OUT)
GPIO.output(MOTOR_VIN, GPIO.LOW)

# Soros port beállítása
serial_port = serial.Serial ("/dev/ttyS0", 9600, timeout=1)

# Ki-és bemeneti puffer ürítése
serial_port.flushInput()
serial_port.flushOutput()
time.sleep(0.5)


host_name = socket.gethostname()
vega_control_socket = ''
ni.ifaddresses('wlan0')
last_instruction = "stop"
serial_active = False
is_video_listen = False
is_control_listen = False
is_connected = False
control_shutdown = False
vega_shutdown = False
vega_ip = ni.ifaddresses('wlan0')[2][0]['addr']
print("VEGA_IP: ", vega_ip)


def control_core():
    global vega_control_socket
    global is_connected
    global vega_ip
    global control_shutdown
    global is_control_listen
    global serial_active
    control_port = 8000
    vega_control_socket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    vega_control_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    vega_control_socket_address = (vega_ip,control_port)
    vega_control_socket.bind(vega_control_socket_address)
    vega_control_socket.listen(1)
    print("Várakozás kliensre(CONTROL_SOCKET): ", vega_control_socket_address)
    vega_control_socket,addr = vega_control_socket.accept()
    is_connected = True
    print("(CONTROL_SOCKET)Csatlakozva: ", addr)
    print("Arduino indítása...")
    serial_active = True
    GPIO.output(ARDUINO_VIN, GPIO.HIGH)
    time.sleep(1)
    print("Motorok engedélyezése...")
    #GPIO.output(MOTOR_VIN, GPIO.HIGH)
    while True:
        try:
            message = vega_control_socket.recv(128)
            if len(message) > 0:
                inst_execution(message)
        except ConnectionResetError:
            print("Távoli irányítás hiba miatt leállt.")
            break
        if control_shutdown == True:
            break
    print("Távoli irányítás leáll.")
    print("Arduino és motorok kikapcsolása.")
    #GPIO.output(MOTOR_VIN, GPIO.LOW)
    GPIO.output(ARDUINO_VIN, GPIO.LOW)
    serial_active = False
    is_connected = False
    try:
        vega_control_socket.shutdown(socket.SHUT_RDWR)
        vega_control_socket.close()
    except OSError:
        pass
    is_control_listen = False
    control_shutdown = False

def video_stream():
        global vega_ip
        global is_video_listen
        global stream_shutdown
        video_stream_socket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        video_port = 9999
        video_stream_socket_address = (vega_ip,video_port)
        video_stream_socket.bind(video_stream_socket_address)
        video_stream_socket.listen(1)
        print("Várakozás kliensre(VIDEO_STREAM): ",video_stream_socket_address)
        while True:
            client_socket,addr = video_stream_socket.accept()
            print("(VIDEO_STREAM)Csatlakozva: ",addr)
            if client_socket:
                vid = cv2.VideoCapture(0)
                while(vid.isOpened()):
                    img,frame = vid.read()
                    a = pickle.dumps(frame)
                    message = struct.pack("Q", len(a))+a
                    try:
                        client_socket.sendall(message)
                    except ConnectionResetError:
                        break
            break
        print("Kamera streaming leáll.")
        client_socket.close()
        is_video_listen = False


def inst_execution(instruction):
    global last_instruction
    global control_shutdown
    global vega_shutdown
    try:
        op,use_param,param = struct.unpack('4s ? f', instruction)
        op = op.decode('ascii')
        if op == "cosp":
            control_shutdown = True
        elif op == "shud":
            vega_shutdown = True
        else:
            op = op + '\n'
            serial_port.write(op.encode('utf-8'))
        time.sleep(0.1)
        if use_param:
            param = str(param) + '\n'
            serial_port.write(param.encode('utf-8'))
            time.sleep(0.1)
            if last_instruction in DIRECTIONS and last_instruction != "stop\n":
                serial_port.write(last_instruction.encode('utf-8'))
        if op in DIRECTIONS:
            last_instruction = op
    except struct.error:
        print("Átviteli hiba.")

def battery_info(adc_num):
    reference = float (14.88 / 660)
    adc_num = int(adc_num)
    voltage = reference * adc_num
    voltage = voltage - 12.8
    percentage = int((voltage / 4) * 100)
    op = "bati"
    info_packet = struct.pack("4s i", bytes(op, 'ascii'), percentage)
    send_info(info_packet)
            

def distance_info(adc_num):
    distance = int((10 * 375) / int(adc_num))
    op = "bari"
    info_packet = struct.pack("4s i", bytes(op, 'ascii'), distance)
    send_info(info_packet)
        

def send_info(packet):
    global vega_control_socket
    global is_connected
    if is_connected:
        vega_control_socket.sendall(packet)


def check_height_step():
    pass

               
while True:        
    if not is_control_listen:
        is_control_listen = True
        threading.Thread(target=control_core).start()  
    if not is_video_listen:
        is_video_listen = True
        threading.Thread(target=video_stream).start()
    if serial_port.in_waiting != 0 and serial_active:
        try:
            arduino = serial_port.readline().decode('utf-8').rstrip()
            if arduino[:3] == "bat":
                battery_info(arduino[3:])
            elif arduino[:3] == "dis":
                distance_info(arduino[3:])
            print(arduino)
        except UnicodeDecodeError:
            pass
    if vega_shutdown:
        break
    time.sleep(0.25)
GPIO.output(MOTOR_VIN, GPIO.LOW)
GPIO.output(ARDUINO_VIN, GPIO.LOW)
GPIO.cleanup()
os.system("sudo shutdown now -h")
