import dearpygui.dearpygui as dpg

def add_str(tag, string):
    if string is None or string == "":
        return

    old_string = dpg.get_value("Display")
    count = old_string.count("\n")
    if count > 20:
        token = old_string.split("\n")
        token.pop(0)
        old_string = "\n".join(token)
    dpg.set_value(tag,  old_string + "\n" + string)