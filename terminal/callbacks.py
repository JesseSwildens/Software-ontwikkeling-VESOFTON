import dearpygui.dearpygui as dpg

from display import add_display, clear_display
from serial_handler import SerialHandler

def command_callback(_sender, _app_data, _user_data : SerialHandler):
    command = dpg.get_value("Command")
    _user_data.send(command)
    add_display(command, True)
    dpg.set_value("Command", "")
    dpg.focus_item("Command")

def button_callback(_sender, _app_data, _user_data : SerialHandler):
    command = dpg.get_value("Command")
    _user_data.send(command)
    add_display(command, True)
    dpg.set_value("Command", "")

def resize_callback(_sender, _app_data, _user_data : SerialHandler):
    dpg.set_item_pos("Send", (10, dpg.get_viewport_height()-75))
    dpg.set_item_pos(
        "Command", 
        (dpg.get_item_width("Send") + 25, dpg.get_viewport_height()-75))
    dpg.set_item_pos(
        "Status", 
        (dpg.get_item_width("Send") + dpg.get_item_width("Command") + 50,
         dpg.get_viewport_height()-75))

def menu_callback(sender, _app_data, user_data : SerialHandler):
    enable_items = ["Command", "Send"]

    if user_data.connect(sender):
        dpg.set_value("Status", "Connected")
        dpg.configure_item("Status", color=(0, 255, 0, 255)) # Red
        clear_display()
        enable = True
    else:
        dpg.set_value("Status", "Disconnected")
        dpg.configure_item("Status", color=(255, 0, 0, 255)) # Green
        enable = False

    for item in enable_items:
        dpg.configure_item(item, enabled=enable)
