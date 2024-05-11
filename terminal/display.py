import dearpygui.dearpygui as dpg

def add_display(string : str, is_echo = False) -> bool:
    """Adds a string to the display box

    Args:
        string (str): String to add
        is_echo (bool, optional): If string is echo, give a different color. 
        Defaults to False.

    Returns:
        bool: Returns True if data got overwritten
    """
    if is_echo:
        color = [255, 255, 0, 255]
    else:
        color = [255, 255, 255, 255]

    idx = 0
    while dpg.is_item_shown(f"_display_{idx}") and \
        (idx*13 < dpg.get_viewport_height() - 130):
        idx += 1

    # In case the box is not full yet
    if not dpg.is_item_shown(f"_display_{idx}"):
        dpg.set_value(f"_display_{idx}", string)
        dpg.configure_item(f"_display_{idx}", show=True)
        return False

    # In case the box is full
    idx = 1
    while (dpg.is_item_shown(f"_display_{idx}")) and \
        (idx*13 < dpg.get_viewport_height() - 130):

        # shift data up
        dpg.set_value(f"_display_{idx}", dpg.get_value(f"_display_{idx+1}"))
        dpg.configure_item(
            f"_display_{idx}", 
            show=True,
            # Color is stored as a float, but can only be set as an int
            # Who the f makes this up
            color=[x*255 for x in
                   dpg.get_item_configuration(f"_display_{idx+1}")['color']])
        idx += 1

    dpg.set_value(f"_display_{idx}", string)
    dpg.configure_item(f"_display_{idx}", show=True, color=color)

    # Trim any text that is out of bounds
    idx += 1
    while dpg.is_item_shown(f"_display_{idx}"):
        dpg.configure_item(f"_display_{idx}", show=False)
        idx += 1
    return True

def init_display():
    """Initialize the display box with text that isn't shown
    """
    for idx in range(150):
        dpg.add_text("", tag=f"_display_{idx}", show=False, pos=(20, 10+idx*13))

def clear_display():
    """Hide all text in the display box
    """
    for idx in range(150):
        dpg.set_value(f"_display_{idx}", "")
