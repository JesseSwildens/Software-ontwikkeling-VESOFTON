import dearpygui.dearpygui as dpg

def add_str(tag, string):
    if string is None or string == "":
        return

    old_string = dpg.get_value("Display")
    count = old_string.count("\n")
    while count > ((dpg.get_viewport_height() - 130) // 13):
        token = old_string.split("\n")
        token.pop(0)
        old_string = "\n".join(token)
        count = old_string.count("\n")
    dpg.set_value(tag,  old_string + "\n" + string)
