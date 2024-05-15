import time

import dearpygui.dearpygui as dpg

from display import add_display, clear_display
from serial_handler import SerialHandler

def find_current_string():
    for idx in range(100):
        color = dpg.get_item_configuration(f"_import_{idx}")['color']
        color = [int(x*255) for x in color] # wonky color :)
        if color == [255, 255, 0, 255]:
            break
    return idx

def highlight_next(tag_without_idx, idx):
    next = 1
    try:
        while dpg.get_value(tag_without_idx + str(idx+next))\
                           .strip("\n").strip("\r") == "":
            next += 1
    except AttributeError:
        next = -idx

    dpg.configure_item(tag_without_idx + str(idx+next), color = [
            int(x*255) for x in 
            dpg.get_item_configuration(tag_without_idx + str(idx))['color']])
    dpg.configure_item(tag_without_idx + str(idx), color = [255, 255, 255, 255])
    return idx+next

def command_callback(_sender, _app_data, user_data : SerialHandler):
    command = dpg.get_value("_command")
    user_data.send(command)
    add_display(command, True)
    dpg.set_value("_command", "")
    dpg.focus_item("_command")

def button_callback(sender, _app_data, user_data : SerialHandler):
    if sender == "_send":
        command = dpg.get_value("_command")
        user_data.send(command)
        add_display(command, True)
        dpg.set_value("_command", "")
    elif sender == "_step_over":
        idx = find_current_string()
        add_display(dpg.get_value(f"_import_{idx}"), True)
        user_data.send(dpg.get_value(f"_import_{idx}"))
        highlight_next("_import_", idx)
    elif sender == "_skip":
        idx = find_current_string()
        highlight_next("_import_", idx)
    elif sender == "_send_all":
        idx = find_current_string()
        while(highlight_next("_import_", idx) != 0):
            idx = find_current_string()
            command = dpg.get_value(f"_import_{idx}")
            user_data.send(command)
            add_display(command, True)
            time.sleep(dpg.get_value("_interval_slider")/1000)

def viewport_resize_callback(_sender):
    dpg.set_item_pos("_send", (10, dpg.get_viewport_height()-75))
    dpg.set_item_pos(
        "_command", 
        (dpg.get_item_width("_send") + 25, dpg.get_viewport_height()-75))
    dpg.set_item_pos(
        "_status", 
        (dpg.get_item_width("_send") + dpg.get_item_width("_command") + 50,
         dpg.get_viewport_height()-75))

def window_resize_callback(_sender):
    dpg.set_item_pos("_step_over", (dpg.get_item_width("_imported_text")-100,
                                    dpg.get_item_height("_imported_text")-90))
    dpg.set_item_pos("_skip", (dpg.get_item_width("_imported_text")-100,
                               dpg.get_item_height("_imported_text")-60))
    dpg.set_item_pos("_send_all", (dpg.get_item_width("_imported_text")-100,
                                   dpg.get_item_height("_imported_text")-30))

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

def ok_callback(_sender, app_data):
    with open(app_data['file_path_name'], 'r') as file:
        for idx, line in enumerate(file):
            dpg.set_value(f"_import_{idx}", line.strip("\r").strip('\n'))
        dpg.configure_item("_imported_text", show=True)
        dpg.configure_item("_import_0", color=(255, 255, 0, 255))
