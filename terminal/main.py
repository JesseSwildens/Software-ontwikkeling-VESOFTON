import dearpygui.dearpygui as dpg

import callbacks
from serial_handler import get_available_ports

dpg.create_context()
dpg.create_viewport(title="Terminal", width=600, height=400, resizable=True)
dpg.set_viewport_resize_callback(callbacks.resize_callback)

# Main window
with dpg.window(tag="Primary"):
    # Create Items
    display_text = dpg.add_text("Lorem ipsum dolor sit amet", tag="Display")
    input_text = dpg.add_input_text(
        tag="Command", on_enter=True, hint="Command")
    button_send = dpg.add_button(tag="Send", label="Send")

    # Attach callbacks
    dpg.set_item_callback("Send", callbacks.button_callback)
    dpg.set_item_callback("Command", callbacks.command_callback)

    # Item positioning
    dpg.set_item_pos("Send", (10, dpg.get_viewport_height()-75))
    dpg.set_item_pos("Command", (70, dpg.get_viewport_height()-75))
    dpg.set_item_pos("Display", (20, 20))

# Menu bar
with dpg.viewport_menu_bar():
    with dpg.menu(label="File"):
        dpg.add_menu_item(label="Save (WIP)")
        dpg.add_menu_item(label="Save as (WIP)")
    with dpg.menu(label="Port"):
        ports = get_available_ports()
        for port in ports:
            dpg.add_menu_item(label=f"{port.name} {port.description}")

dpg.show_style_editor()
dpg.setup_dearpygui()
dpg.show_viewport()
dpg.set_primary_window("Primary", True)

while dpg.is_dearpygui_running():
    dpg.render_dearpygui_frame()

dpg.destroy_context()
