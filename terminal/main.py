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
dpg.create_viewport(title="Terminal", width=960, height=720, resizable=True)
dpg.set_viewport_resize_callback(callbacks.viewport_resize_callback)

# Main window
with dpg.window(tag="_primary"):
    # Create Items
    dpg.add_input_text(
        tag="_command", on_enter=True, hint="Command",
        enabled=False, width=500)
    dpg.add_button(tag="_send", label="Send", enabled=False, width=100)
    dpg.add_text("Disconnected", tag="_status", color=(255, 0, 0, 255))

    # Create display box
    init_display()
    while not add_display(""):
        # Add empty lines to start displaying text at the bottom instead of top
        pass

    # Attach callbacks
    dpg.set_item_callback("_send", callbacks.button_callback)
    dpg.set_item_user_data("_send", ser)
    dpg.set_item_callback("_command", callbacks.command_callback)
    dpg.set_item_user_data("_command", ser)

    # Item positioning
    dpg.set_item_pos("_send", (10, dpg.get_viewport_height()-75))
    dpg.set_item_pos("_command", (70, dpg.get_viewport_height()-75))
    dpg.set_item_pos(
        "_status", 
        (dpg.get_viewport_width()-len(dpg.get_value("_status"))-100, 20))

# File dialog
with dpg.file_dialog(directory_selector=False, show=False,
                    callback=callbacks.ok_callback, tag="_file_dialog",
                    width=700 ,height=400):
    dpg.add_file_extension(".txt", color=(255, 255, 0, 255))

# Window for imported text.
with dpg.window(tag="_imported_text", show=False, autosize=False,
                width=700, height=400, no_scrollbar=True):
    # Add lines for imported text
    for idx in range(100):
        dpg.add_text("", tag=f"_import_{idx}", pos=(20, 20+idx*13))

    # Add buttons
    dpg.add_button(
        label="Step over",
        tag="_step_over",
        pos=(
            dpg.get_item_width("_imported_text")-100,
            dpg.get_item_height("_imported_text")-90),
        width=80,
        callback=callbacks.button_callback,
        user_data=ser)
    dpg.add_button(
        label="Skip",
        tag="_skip",
        pos=(
            dpg.get_item_width("_imported_text")-100,
            dpg.get_item_height("_imported_text")-60),
        width=80,
        callback=callbacks.button_callback,
        user_data=ser)
    dpg.add_button(
        label="Send all",
        tag="_send_all",
        pos=(
            dpg.get_item_width("_imported_text")-100,
            dpg.get_item_height("_imported_text")-30),
        width=80,
        callback=callbacks.button_callback,
        user_data=ser)

# Handler for window resizing (instead of viewport)
with dpg.item_handler_registry(tag="_window_handler"):
    dpg.add_item_resize_handler(callback=callbacks.window_resize_callback)
dpg.bind_item_handler_registry("_imported_text", "_window_handler")

# Menu bar
with dpg.viewport_menu_bar():
    with dpg.menu(label="File"):
        dpg.add_menu_item(label="Open",
                          callback=lambda:dpg.show_item("_file_dialog"))
    with dpg.menu(label="Port"):
        for port in ser.available_ports:
            dpg.add_menu_item(
                tag = f"{port.device}",
                label=f"{port.device} {port.description}",
                callback=callbacks.menu_callback)
            dpg.set_item_user_data(f"{port.device}", ser)
    with dpg.menu(label="Tools"):
        dpg.add_menu_item(
            label="Show Metrics",
            callback=lambda:dpg.show_tool(dpg.mvTool_Metrics))
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
dpg.set_primary_window("_primary", True)

# Render loop
while dpg.is_dearpygui_running():
    data = ser.poll()
    if data is not None:
        add_display(data)
    dpg.render_dearpygui_frame()

dpg.destroy_context()
