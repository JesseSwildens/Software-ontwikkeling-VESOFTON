import dearpygui.dearpygui as dpg

from display import add_display, clear_display
from serial_handler import SerialHandler

def find_current_string():
    tag = ""
    for idx in range(100):
        color = dpg.get_item_configuration(f"_import_{idx}")['color']
        color = [int(x*255) for x in color] # wonky color :)
        print(color)
        if color == [255, 255, 0, 255]:
            break

    return tag

def command_callback(_sender, _app_data, _user_data : SerialHandler):
    command = dpg.get_value("_command")
    _user_data.send(command)
    add_display(command, True)
    dpg.set_value("_command", "")
    dpg.focus_item("_command")

def button_callback(_sender, _app_data, _user_data : SerialHandler):
    print(_sender)
    if _sender == "_send":
        command = dpg.get_value("_command")
        _user_data.send(command)
        add_display(command, True)
        dpg.set_value("_command", "")
    elif _sender == "_step_over":
        print("_step_over called")
        find_current_string()
    elif _sender == "_skip":
        pass
    elif _sender == "_send_all":
        pass

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
