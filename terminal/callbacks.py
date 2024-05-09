import dearpygui.dearpygui as dpg

from helpers import add_str

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