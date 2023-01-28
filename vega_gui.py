import sys
import time
import cv2 as cv
import numpy as np
import socket
import pickle
import struct
import dearpygui.dearpygui as gui
import threading
import constants as const
import gesture_recognizer
import image_write as imw

INST_FORMAT = "4s ? f"
INF_FORMAT = "4s i"

gui.create_context()
gui.create_viewport(title='VEGA Quadruped Robot', width=1280, height=720)
gui.setup_dearpygui()

# Csatlakozás adatok
gui_control_socket = ""
vega_ip = ""
video_port = 9999
control_port = 8000

last_instruction = ""
last_gesture = ""
ml_model_path = "./data/v_ml_model.dat"
is_connected = False
gesture_active = False
show_cam = False

vega_logo_raw = imw.image_writer('./data/images/vega_logo.png')
tf_logo_raw = imw.image_writer('./data/images/tf_logo.png')
keras_logo_raw = imw.image_writer('./data/images/keras_logo.png')
media_logo_raw = imw.image_writer('./data/images/mediapipe_logo.png')
opencv_logo_raw = imw.image_writer('./data/images/opencv_logo.png')
video_image_raw = imw.image_writer('./data/images/streaming_logo.png')
vega_logo_dim = [570, 200]
tf_logo_dim = [100, 23]
keras_logo_dim = [100, 29]
media_logo_dim = [100, 18]
opencv_logo_dim = [100, 123]
vid_dim = [480, 640]


def change_gui_lookout(layout):
    if layout == "control":
        gui.show_item("moving_text")
        gui.show_item("upper_line")
        for o in const.DIRECTIONS:
            gui.show_item(o)

        gui.show_item("settings_text")
        gui.show_item("bottom_line")
        for o in const.SETTINGS:
            gui.show_item(o)

        gui.show_item("camera_window")

        gui.hide_item("no_connection")
        gui.show_item("connection_ok")

        for o in const.BATTERY_LOGO:
            gui.show_item(o)

        for o in const.LOGOS:
            gui.hide_item(o)

        for o in const.BARRIER_LOGO:
            if o != const.BARRIER_LOGO[3]:
                gui.show_item(o)

    else:
        gui.hide_item("moving_text")
        gui.hide_item("upper_line")
        for o in const.DIRECTIONS:
            gui.hide_item(o)

        gui.hide_item("settings_text")
        gui.hide_item("bottom_line")
        for o in const.SETTINGS:
            gui.hide_item(o)

        gui.hide_item("connection_ok")
        gui.show_item("no_connection")
        gui.hide_item(camera_window)

        for o in const.BATTERY_LOGO:
            gui.hide_item(o)

        for o in const.BARRIER_LOGO:
            gui.hide_item(o)

        for o in const.LOGOS:
            gui.show_item(o)


def window_open(sender):
    if sender == "camera":
        gui.show_item("camera_window")
    elif sender == "control":
        gui.show_item("control_window")


def search_ip():
    global vega_ip
    try:
        print("VEGA keresése a hálózaton...")
        vega_ip = socket.gethostbyname("vegarobot")
        print("VEGA IP-címe: ", vega_ip)
        gui.hide_item("welcome_1")
        gui.hide_item("welcome_2")
        gui.hide_item("ip_search")
        gui.show_item("connect")
    except socket.gaierror:
        print("Az adott hálózaton nem található VEGA!")


def camera_toggle():
    global show_cam
    if not show_cam:
        gui.configure_item("camera_button", label="Kamera leállítása")
        threading.Thread(target=streaming).start()
    else:
        gui.configure_item("camera_button", label="Kamera indítása")
    show_cam = not show_cam


def op_control(sender):
    if sender in const.DIRECTIONS:
        send_instruction(sender, False, 0.0)
    elif sender in const.SETTINGS and (sender != const.VEGA_SHUTDOWN):
        if sender == const.MOTION_DELAY:
            send_instruction(sender, True, gui.get_value(sender))
        elif sender == const.A_TURN:
            send_instruction(sender, True, gui.get_value(sender))
        else:
            send_instruction(sender, True, 10 * float(gui.get_value(sender)))
    elif sender == "connect":
        send_instruction(const.CON_STOP, False, 0.0)
    elif sender == const.VEGA_SHUTDOWN:
        send_instruction(const.VEGA_SHUTDOWN, False, 0.0)
        change_gui_lookout("welcome")
        gui.hide_item("connect")
        gui.show_item("ip_search")
        gui.show_item("welcome_1")
        gui.show_item("welcome_2")
    show_active_button(sender)


def show_active_button(button):
    global last_instruction
    if button != "connect":
        gui.bind_item_theme(button, active_button)
    if last_instruction != "":
        gui.bind_item_theme(last_instruction, theme='')
    last_instruction = button


def send_instruction(op_code, use_parameter, parameter):
    global gui_control_socket
    # Utasítás készítése
    inst_packet = struct.pack(INST_FORMAT, bytes(op_code, 'ascii'), use_parameter, parameter)
    gui_control_socket.sendall(inst_packet)


def telemetry():
    global gui_control_socket
    global is_connected
    while True:
        try:
            inf_packet = gui_control_socket.recv(128)
            if len(inf_packet) > 0:
                datatype, inf_data = struct.unpack(INF_FORMAT, inf_packet)
                datatype = datatype.decode('ascii')
                if datatype == const.BATTERY_INFO:
                    battery_logo(inf_data)
                elif datatype == const.BARRIER_INFO:
                    barrier_logo(inf_data)
                elif datatype == const.A_HEIGHT:
                    gui.set_value(const.A_HEIGHT, inf_data)
                elif datatype == const.A_STEP:
                    gui.set_value(const.A_STEP, inf_data)
                elif datatype == const.MOTION_DELAY:
                    gui.set_value(const.MOTION_DELAY, inf_data)
        except BrokenPipeError:
            break
        except OSError:
            break
    print("Telemetria leáll!")


def battery_logo(number):
    gui.set_value("battery_charge", str(number) + "%")
    if number < 40:
        for o in const.BATTERY_LOGO:
            gui.configure_item(o, color=(255, 255, 0, 255))
    elif number < 15:
        for o in const.BATTERY_LOGO:
            gui.configure_item(o, color=(255, 0, 0, 255))
    else:
        for o in const.BATTERY_LOGO:
            gui.configure_item(o, color=(0, 255, 0, 255))


def barrier_logo(number):
    gui.set_value("distance", str(number) + "cm")
    if number < 10:
        for o in const.BARRIER_LOGO:
            gui.configure_item(o, color=(255, 0, 0, 255))
    elif number < 15:
        for o in const.BARRIER_LOGO:
            gui.configure_item(o, color=(255, 255, 0, 255))
    else:
        for o in const.BARRIER_LOGO:
            gui.configure_item(o, color=(0, 255, 0, 255))


def control_connect():
    global gui_control_socket
    global is_connected
    if not is_connected:
        try:
            gui_control_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            gui_control_socket.connect((vega_ip, control_port))
            change_gui_lookout("control")
            gui.configure_item("connect", label="Kapcsolat bontása")
            threading.Thread(target=telemetry).start()
        except ConnectionRefusedError:
            print("Hiba a kapcsolat létrehozásakor!")
    else:
        op_control("connect")
        time.sleep(1)
        gui_control_socket.shutdown(socket.SHUT_RDWR)
        gui_control_socket.close()
        print("Kapcsolat sikeresen bontva!")
        change_gui_lookout("welcome")
        gui.configure_item("connect", label="Csatlakozás")
    is_connected = not is_connected


def gesture_control_launch():
    global gesture_active
    if not gesture_active:
        gui.bind_item_theme(const.GESTURE_BUTTON, active_button)
        recognizer = gesture_recognizer.GestureRecognizer(model_path=ml_model_path)
        threading.Thread(target=gesture_core, args=(recognizer,)).start()
    else:
        gui.bind_item_theme(const.GESTURE_BUTTON, theme='')
        gesture_active = not gesture_active


def gesture_core(recognizer):
    global gesture_active
    global last_gesture
    gesture_active = True
    vid = cv.VideoCapture(0)
    if not vid.isOpened():
        print("Hiba a kamera indításakor!")
        sys.exit()
    while gesture_active:
        ret, image = vid.read()
        image2 = cv.rectangle(image, (200, 100), (440, 380), (0, 255, 0), 3)
        new_gesture = recognizer.recognize(image=image)
        cv.imshow("Recognizer", image2)
        cv.waitKey(1)
        if new_gesture != last_gesture and (new_gesture != "None!"):
            op_control(const.GESTURE_CONTROL[new_gesture])
            print(const.GESTURE_CONTROL[new_gesture])
            last_gesture = new_gesture
    print("Gesztusvezérlés leáll.")
    vid.release()


def wifi_migrate():
    pass


def streaming():
    global vega_ip
    global show_cam
    gui_video_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    gui_video_socket.connect((vega_ip, video_port))
    b_data = b""
    payload_size = struct.calcsize("Q")
    while True:
        while len(b_data) < payload_size:
            packet = gui_video_socket.recv(4 * 1024)
            if not packet:
                break
            b_data += packet
        packed_msg_size = b_data[:payload_size]
        b_data = b_data[payload_size:]
        msg_size = struct.unpack("Q", packed_msg_size)[0]
        while len(b_data) < msg_size:
            b_data += gui_video_socket.recv(4 * 1024)
        frame_data = b_data[:msg_size]
        b_data = b_data[msg_size:]
        frame = pickle.loads(frame_data)
        frame = cv.rotate(frame, cv.ROTATE_90_CLOCKWISE)
        frame = cv.cvtColor(frame, cv.COLOR_BGR2RGB)
        frame = cv.cvtColor(frame, cv.COLOR_RGB2RGBA)
        frame = frame.ravel()
        frame = np.asfarray(frame, dtype='f')
        texture_data = np.true_divide(frame, 255.0)
        gui.set_value("video_stream", texture_data)
        if not show_cam:
            break
    gui_video_socket.shutdown(socket.SHUT_RDWR)
    gui_video_socket.close()
    sys.exit()


with gui.texture_registry():
    gui.add_raw_texture(vid_dim[0], vid_dim[1], video_image_raw, tag="video_stream", format=gui.mvFormat_Float_rgba)
    gui.add_raw_texture(vega_logo_dim[0], vega_logo_dim[1], vega_logo_raw, tag="vega_logo",
                        format=gui.mvFormat_Float_rgba)
    gui.add_raw_texture(tf_logo_dim[0], tf_logo_dim[1], tf_logo_raw, tag="tf_logo", format=gui.mvFormat_Float_rgba)
    gui.add_raw_texture(keras_logo_dim[0], keras_logo_dim[1], keras_logo_raw, tag="keras_logo",
                        format=gui.mvFormat_Float_rgba)
    gui.add_raw_texture(media_logo_dim[0], media_logo_dim[1], media_logo_raw, tag="media_logo",
                        format=gui.mvFormat_Float_rgba)
    gui.add_raw_texture(opencv_logo_dim[0], opencv_logo_dim[1], opencv_logo_raw, tag="opencv_logo",
                        format=gui.mvFormat_Float_rgba)

with gui.font_registry():
    with gui.font("Righteous-Regular.ttf", 24) as default_font:
        gui.add_font_range(0x0020, 0xFFFF)

with gui.handler_registry():
    gui.add_mouse_release_handler(callback=op_control)

with gui.window(tag="Primary Window", width=1280, height=720):
    gui.bind_font(default_font)
    gui.add_image("vega_logo", tag="logo", pos=(620, 220))
    with gui.theme() as active_button:
        with gui.theme_component(gui.mvAll):
            gui.add_theme_color(gui.mvThemeCol_Button, (150, 100, 100), category=gui.mvThemeCat_Core)

    with gui.menu_bar():
        with gui.menu(tag="menu", label="Menü"):
            gui.add_menu_item(tag="control", label="Irányítás", callback=window_open)
            gui.add_menu_item(tag="camera", label="Kamera", show=False, callback=window_open)
            gui.add_menu_item(tag="wifi_migr", label="Hálózat váltás", callback=wifi_migrate, show=False)

    with gui.window(label="Irányítás", pos=(0, 30), width=540, height=700, no_resize=True,
                    tag="control_window") as control_window:
        gui.add_image("tf_logo", pos=(20, 622), show=True, tag=const.TENSOR_LOGO)
        gui.add_image("keras_logo", pos=(150, 615), show=True, tag=const.KERAS_LOGO)
        gui.add_image("media_logo", pos=(260, 625), show=True, tag=const.MEDIA_LOGO)
        gui.add_image("opencv_logo", pos=(430, 530), show=True, tag=const.OPENCV_LOGO)
        gui.add_text("Kapcsolat állapota: ", pos=(10, 35))
        gui.add_text("Nincs kapcsolat!", pos=(230, 35), tag="no_connection", color=(255, 0, 0))
        gui.add_text("Csatlakozva!", pos=(230, 35), tag="connection_ok", color=(0, 255, 0), show=False)
        gui.add_text("VEGA IP-cím: ", tag="_ip", show=False, pos=(10, 80))
        gui.add_text("Üdvözöllek!", tag="welcome_1", pos=(195, 170), show=True)
        gui.add_text("A kezdéshez kattints a keresés gombra!",
                     tag="welcome_2", pos=(80, 200), show=True)
        gui.add_button(label="Keresés", callback=search_ip, tag="ip_search", pos=(210, 250), show=True)
        gui.add_button(label="Csatlakozás", tag="connect", callback=control_connect, pos=(10, 70), show=False)
        gui.add_button(label="Kapcsolat bontása", pos=(5, 75), tag="terminate", show=False, callback=control_connect)
        gui.add_text(label="Mozgás", pos=(230, 90), tag="moving_text", show=False)
        gui.draw_line((10, 80), (515, 80), color=(255, 255, 255, 255), thickness=3, tag="upper_line", show=False)
        gui.add_button(label="Előre", pos=(240, 190), tag=const.FORWARD, show=False, callback=op_control)
        gui.add_button(label="Hátra", pos=(236, 290), tag=const.BACKWARD, callback=op_control, show=False)
        gui.add_button(label="Balra", pos=(150, 240), tag=const.LEFT, show=False, callback=op_control)
        gui.add_button(label="Stop", pos=(242, 240), tag=const.STOP, show=False, callback=op_control)
        gui.add_button(label="Jobbra", pos=(320, 240), tag=const.RIGHT, show=False, callback=op_control)
        gui.add_button(label="<\n<\n<", pos=(110, 215), tag=const.SIDDLE_LEFT, show=False, callback=op_control)
        gui.add_button(label=">\n>\n>", pos=(415, 215), tag=const.SIDDLE_RIGHT, show=False, callback=op_control)
        gui.draw_arrow((485, 150), (405, 150), color=(255, 255, 255), thickness=3, size=10, tag=const.BARRIER_LOGO[0],
                       show=False)
        gui.draw_line((500, 165), (500, 120), color=(255, 255, 255, 255), thickness=4, tag=const.BARRIER_LOGO[1],
                      show=False)
        gui.add_text("?cm", tag=const.BARRIER_LOGO[2], pos=(420, 155), show=False)
        gui.add_text("Akadály észlelve!", tag=const.BARRIER_LOGO[3], pos=(230, 150), color=(255, 0, 0, 255), show=False)

        gui.draw_line((425, 14), (480, 14), color=(255, 255, 255, 255), thickness=2.5, tag=const.BATTERY_LOGO[0],
                      show=False)
        gui.draw_line((425, 14), (425, 42), color=(255, 255, 255, 255), thickness=2.5, tag=const.BATTERY_LOGO[1],
                      show=False)
        gui.draw_line((425, 41), (481, 41), color=(255, 255, 255, 255), thickness=2.5, tag=const.BATTERY_LOGO[2],
                      show=False)
        gui.draw_line((480, 42), (480, 13), color=(255, 255, 255, 255), thickness=2.5, tag=const.BATTERY_LOGO[3],
                      show=False)
        gui.draw_line((480, 35), (486, 35), color=(255, 255, 255, 255), thickness=2.5, tag=const.BATTERY_LOGO[4],
                      show=False)
        gui.draw_line((480, 20), (487, 20), color=(255, 255, 255, 255), thickness=2.5, tag=const.BATTERY_LOGO[5],
                      show=False)
        gui.draw_line((486, 20), (486, 36), color=(255, 255, 255, 255), thickness=2.5, tag=const.BATTERY_LOGO[6],
                      show=False)
        gui.add_text("?%", tag=const.BATTERY_LOGO[7], pos=(440, 50), color=(255, 255, 255, 255), show=False)

        gui.add_text("Beállítások", pos=(220, 350), tag="settings_text", show=False)
        gui.draw_line((10, 340), (515, 340), color=(255, 255, 255, 255), thickness=3, tag="bottom_line", show=False)
        gui.add_button(label="Leállítás", pos=(18, 400), tag=const.VEGA_SHUTDOWN, show=False, callback=op_control)
        gui.add_button(label="VeGesture", pos=(140, 400), tag=const.GESTURE_BUTTON, show=False,
                       callback=gesture_control_launch)
        gui.add_button(tag=const.CAMERA, label="Kamera indítása", callback=camera_toggle, pos=(280, 400), show=False)
        gui.add_slider_int(label="Magasság(cm)", tag=const.A_HEIGHT, min_value=7, max_value=16, default_value=8,
                           pos=(20, 450), show=False, callback=op_control)
        gui.add_slider_int(label="Lépéshossz(cm)", tag=const.A_STEP, min_value=1, max_value=3, default_value=2,
                           pos=(20, 500), show=False, callback=op_control)
        gui.add_slider_int(label="Fordulás(mm)", tag=const.A_TURN, min_value=5, max_value=15, default_value=5,
                           pos=(20, 550), show=False, callback=op_control)
        gui.add_slider_int(label="Késleltetés(ms)", tag=const.MOTION_DELAY, min_value=0, max_value=25, default_value=20,
                           pos=(20, 600), show=False, callback=op_control)

    with gui.window(label="VEGA_EYE (RPi Kamera)", pos=(560, 30), width=700, height=700, no_resize=True, show=False,
                    tag="camera_window") as camera_window:
        gui.add_image("video_stream", pos=(112, 40), tag="video")

        gui.set_primary_window("Primary Window", True)
gui.show_viewport()
while gui.is_dearpygui_running():
    if is_connected:
        pass
    if not show_cam:
        gui.set_value("video_stream", video_image_raw)
    gui.render_dearpygui_frame()
gui.destroy_context()
