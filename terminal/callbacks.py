import dearpygui.dearpygui as dpg

from display import add_display, clear_display
from serial_handler import SerialHandler

def command_callback(_sender, _app_data, _user_data : SerialHandler):
    command = dpg.get_value("_command")
    _user_data.send(command)
    add_display(command, True)
    dpg.set_value("_command", "")
    dpg.focus_item("_command")

def button_callback(_sender, _app_data, _user_data : SerialHandler):
    command = dpg.get_value("_command")
    _user_data.send(command)
    add_display(command, True)
    dpg.set_value("_command", "")

def resize_callback(_sender, _app_data, _user_data : SerialHandler):
    dpg.set_item_pos("_send", (10, dpg.get_viewport_height()-75))
    dpg.set_item_pos(
        "_command", 
        (dpg.get_item_width("_send") + 25, dpg.get_viewport_height()-75))
    dpg.set_item_pos(
        "_status", 
        (dpg.get_item_width("_send") + dpg.get_item_width("_command") + 50,
         dpg.get_viewport_height()-75))

def menu_callback(sender, _app_data, user_data : SerialHandler):
    enable_items = ["_command", "_send"]

    if user_data.connect(sender):
        dpg.set_value("_status", "Connected")
        dpg.configure_item("_status", color=(0, 255, 0, 255)) # Red
        clear_display()
        enable = True
    else:
        dpg.set_value("_status", "Disconnected")
        dpg.configure_item("_status", color=(255, 0, 0, 255)) # Green
        enable = False

    for item in enable_items:
        dpg.configure_item(item, enabled=enable)
