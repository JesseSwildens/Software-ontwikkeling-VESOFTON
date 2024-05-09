import dearpygui.dearpygui as dpg

from helpers import add_str
from serial_handler import SerialHandler

def command_callback():
    command = dpg.get_value("Command")
    dpg.set_value("Command", "")
    dpg.focus_item("Command")
    add_str("Display", "\t"+command)

def button_callback():
    add_str("Display", dpg.get_value("Command"))

def resize_callback():
    dpg.set_item_pos("Send", (10, dpg.get_viewport_height()-75))
    dpg.set_item_pos("Command", (75, dpg.get_viewport_height()-75))
    dpg.set_item_pos("Display", (20, 20))

def menu_callback(sender, _app_data, user_data : SerialHandler):
    if user_data.connect(sender):
        dpg.set_value("Status", "Connected")
        dpg.configure_item("Status", color=(0, 255, 0, 255)) # Red
    else:
        dpg.set_value("Status", "Disconnected")
        dpg.configure_item("Status", color=(255, 0, 0, 255)) # Green
