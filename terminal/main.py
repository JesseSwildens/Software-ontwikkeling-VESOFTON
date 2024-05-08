import dearpygui.dearpygui as dpg
from helpers import add_str

def command_callback():
    command = dpg.get_value("Command")
    dpg.set_value("Command", "")
    dpg.focus_item("Command")
    add_str("Display", "\t"+command)

def button_callback():
    add_str("Display", dpg.get_value("Command"))

dpg.create_context()
dpg.create_viewport(title="Terminal", width=600, height=400, resizable=False)

with dpg.window(tag="Primary"):
    # Create Items
    display_text = dpg.add_text("Lorem ipsum dolor sit amet", tag="Display")
    input_text = dpg.add_input_text(
        tag="Command", on_enter=True, hint="Command")
    button_send = dpg.add_button(tag="Send", label="Send")

    # Attach callbacks
    dpg.set_item_callback("Send", button_callback)
    dpg.set_item_callback("Command", command_callback)

    # Item positioning
    dpg.set_item_pos("Send", (10, dpg.get_viewport_height()-75))
    dpg.set_item_pos("Command", (70, dpg.get_viewport_height()-75))

dpg.setup_dearpygui()
dpg.show_viewport()
dpg.set_primary_window("Primary", True)

while dpg.is_dearpygui_running():
    dpg.render_dearpygui_frame()

dpg.destroy_context()
