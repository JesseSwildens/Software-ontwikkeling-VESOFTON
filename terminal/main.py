import sys
import dearpygui.dearpygui as dpg

import callbacks
from serial_handler import SerialHandler
from display import init_display, add_display

if not sys.version_info >= (3, 6):
    raise EnvironmentError("Python version too old")

# Serial
ser = SerialHandler()

# set up a viewport
dpg.create_context()
dpg.create_viewport(title="Terminal", width=1280, height=960, resizable=True)
dpg.set_viewport_resize_callback(callbacks.resize_callback)

# Main window
with dpg.window(tag="Primary"):
    # Create Items
    dpg.add_input_text(
        tag="Command", on_enter=True, hint="Command", enabled=False, width=500)
    dpg.add_button(tag="Send", label="Send", enabled=False, width=100)
    dpg.add_text("Disconnected", tag="Status", color=(255, 0, 0, 255))

    # Create display box
    init_display()
    while not add_display(""):
        # Add empty lines to start displaying text at the bottom instead of top
        pass

    # Attach callbacks
    dpg.set_item_callback("Send", callbacks.button_callback)
    dpg.set_item_user_data("Send", ser)
    dpg.set_item_callback("Command", callbacks.command_callback)
    dpg.set_item_user_data("Command", ser)

    # Item positioning
    dpg.set_item_pos("Send", (10, dpg.get_viewport_height()-75))
    dpg.set_item_pos("Command", (70, dpg.get_viewport_height()-75))
    # dpg.set_item_pos("Display", (20, 20))
    dpg.set_item_pos(
        "Status", 
        (dpg.get_viewport_width()-len(dpg.get_value("Status"))-100, 20))

# Menu bar
with dpg.viewport_menu_bar():
    with dpg.menu(label="File"):
        dpg.add_menu_item(label="Save (WIP)")
        dpg.add_menu_item(label="Save as (WIP)")
    with dpg.menu(label="Port"):
        for port in ser.available_ports:
            dpg.add_menu_item(
                tag = f"{port.device}",
                label=f"{port.device} {port.description}",
                callback=callbacks.menu_callback)
            dpg.set_item_user_data(f"{port.device}", ser)
    with dpg.menu(label="Tools"):
        dpg.add_menu_item(
            label="Show About", callback=lambda:dpg.show_tool(dpg.mvTool_About))
        dpg.add_menu_item(
            label="Show Metrics",
            callback=lambda:dpg.show_tool(dpg.mvTool_Metrics))
        dpg.add_menu_item(
            label="Show Documentation",
            callback=lambda:dpg.show_tool(dpg.mvTool_Doc))
        dpg.add_menu_item(
            label="Show Debug", callback=lambda:dpg.show_tool(dpg.mvTool_Debug))
        dpg.add_menu_item(
            label="Show Style Editor",
            callback=lambda:dpg.show_tool(dpg.mvTool_Style))
        dpg.add_menu_item(
            label="Show Font Manager",
            callback=lambda:dpg.show_tool(dpg.mvTool_Font))
        dpg.add_menu_item(
            label="Show Item Registry",
            callback=lambda:dpg.show_tool(dpg.mvTool_ItemRegistry))

dpg.setup_dearpygui()
dpg.show_viewport()
dpg.set_primary_window("Primary", True)

while dpg.is_dearpygui_running():
    data = ser.poll()
    if data is not None:
        add_display(data)
    dpg.render_dearpygui_frame()

dpg.destroy_context()
