import dearpygui.dearpygui as dpg

def add_str(tag, string):
    if string is None or string == "":
        return

    old_string = dpg.get_value("Display")
    count = old_string.count("\n")
    if count > 21:
        token = old_string.split("\n")
        token.pop(0)
        old_string = "\n".join(token)
    dpg.set_value(tag,  old_string + "\n" + string)

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
dpg.start_dearpygui()
dpg.destroy_context()
