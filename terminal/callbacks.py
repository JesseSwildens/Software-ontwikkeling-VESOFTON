import os.path
import time

import dearpygui.dearpygui as dpg
from bitmap import BitmapGenerator
from display import add_display, clear_display
from serial_handler import SerialHandler
import stream


def find_current_string():
    for idx in range(100):
        color = dpg.get_item_configuration(f'_import_{idx}')['color']
        color = [int(x * 255) for x in color]  # wonky color :)
        if color == [255, 255, 0, 255]:
            break
    return idx


def highlight_next(tag_without_idx, idx):
    next = 1
    try:
        while (
            dpg.get_value(tag_without_idx + str(idx + next))
            .strip('\n')
            .strip('\r')
            == ''
        ):
            next += 1
    except AttributeError:
        next = -idx

    dpg.configure_item(
        tag_without_idx + str(idx + next),
        color=[
            int(x * 255)
            for x in dpg.get_item_configuration(tag_without_idx + str(idx))[
                'color'
            ]
        ],
    )
    dpg.configure_item(tag_without_idx + str(idx), color=[255, 255, 255, 255])
    return idx + next


def command_callback(_sender, _app_data, user_data: SerialHandler):
    command = dpg.get_value('_command')
    user_data.send(command)
    add_display(command, True)
    dpg.set_value('_command', '')
    dpg.focus_item('_command')


def button_callback(sender, _app_data, user_data):
    if sender == '_send':
        command = dpg.get_value('_command')
        user_data.send(command)
        add_display(command, True)
        dpg.set_value('_command', '')
    elif sender == '_step_over':
        idx = find_current_string()
        add_display(dpg.get_value(f'_import_{idx}'), True)
        user_data.send(dpg.get_value(f'_import_{idx}'))
        highlight_next('_import_', idx)
    elif sender == '_skip':
        idx = find_current_string()
        highlight_next('_import_', idx)
    elif sender == '_send_all':
        idx = find_current_string()
        while highlight_next('_import_', idx) != 0:
            command = dpg.get_value(f'_import_{idx}')
            user_data.send(command)
            add_display(command, True)
            time.sleep(dpg.get_value('_interval_slider') / 1000)
            idx = find_current_string()
        # Force the last line to be sent as well
        command = dpg.get_value(f'_import_{idx+1}')
        user_data.send(command)
    elif sender == '_convert':
        # user_data is a path in this case
        bmap = BitmapGenerator()
        path = dpg.get_item_user_data('_preview_image')
        name = path.split('\\')[-1]
        img = bmap.convert_path_bitmap(
            path, int(dpg.get_value('_target_size_image'))
        )
        bmap.write_to_file(img, name)
    else:
        print(sender)


def viewport_resize_callback(_sender):
    dpg.set_item_pos('_send', (10, dpg.get_viewport_height() - 75))
    dpg.set_item_pos(
        '_command',
        (dpg.get_item_width('_send') + 25, dpg.get_viewport_height() - 75),
    )
    dpg.set_item_pos(
        '_status',
        (
            dpg.get_item_width('_send') + dpg.get_item_width('_command') + 50,
            dpg.get_viewport_height() - 75,
        ),
    )


def window_resize_callback(_sender):
    dpg.set_item_pos(
        '_step_over',
        (
            dpg.get_item_width('_imported_text') - 100,
            dpg.get_item_height('_imported_text') - 90,
        ),
    )
    dpg.set_item_pos(
        '_skip',
        (
            dpg.get_item_width('_imported_text') - 100,
            dpg.get_item_height('_imported_text') - 60,
        ),
    )
    dpg.set_item_pos(
        '_send_all',
        (
            dpg.get_item_width('_imported_text') - 100,
            dpg.get_item_height('_imported_text') - 30,
        ),
    )


def menu_callback(sender, _app_data, user_data: SerialHandler):
    enable_items = ['_command', '_send']
    show_items = ['_menu_stream', '_menu_open_stream']

    user_data.ser.baudrate = 115200
    if user_data.connect(sender):
        dpg.set_value('_status', 'Connected')
        dpg.configure_item('_status', color=(0, 255, 0, 255))  # Red
        clear_display()
        state = True
    else:
        dpg.set_value('_status', 'Disconnected')
        dpg.configure_item('_status', color=(255, 0, 0, 255))  # Green
        state = False

    for item in show_items:
        dpg.configure_item(item, show=state)

    for item in enable_items:
        dpg.configure_item(item, enabled=state)


def warning_popup():
    with dpg.window(
        label='Warning', modal=True, tag='_warning', width=250, height=150
    ):
        dpg.add_text('Something went wrong!')
        dpg.add_button(label='OK', callback=lambda: dpg.delete_item('_warning'))


def ok_callback(_sender, app_data, _user_data):
    path = app_data['file_path_name']

    # Generic check if a file exists
    if not os.path.isfile(path):
        warning_popup()
        return

    if path.endswith('.txt'):
        with open(app_data['file_path_name'], 'r') as file:
            # remove old lines
            for idx in range(100):
                dpg.set_value(f'_import_{idx}', '')

            # add imported data
            for idx, line in enumerate(file):
                dpg.set_value(f'_import_{idx}', line.strip('\r').strip('\n'))

            # highlight the first string
            dpg.configure_item('_import_0', color=(255, 255, 0, 255))

            # show the window
            dpg.configure_item('_imported_text', show=True)

    elif path.endswith('.jpeg') or path.endswith('.png'):
        width, height, _channels, data = dpg.load_image(path)

        # Delete the previous texture + image if it exists
        if dpg.does_item_exist('_image'):
            dpg.delete_item('_image')
            dpg.delete_item('_preview_image')

        # Add the texture to the registry
        with dpg.texture_registry(show=False):
            dpg.add_static_texture(
                width=width, height=height, default_value=data, tag='_image'
            )

        # Find a reasonable sclaer
        w_factor = 320 / dpg.get_item_width('_image')
        h_factor = 240 / dpg.get_item_height('_image')
        scale_factor = min(w_factor, h_factor)

        # add the image to the window
        dpg.add_image(
            '_image',
            tag='_preview_image',
            parent='_preview_window',
            width=int(dpg.get_item_width('_image') * scale_factor),
            height=int(dpg.get_item_width('_image') * scale_factor),
            before='_convert',
            user_data=path,
        )

        # Adjust the window accordingly
        dpg.configure_item(
            '_preview_window', show=True, label=path.split('\\')[-1]
        )
    else:
        warning_popup()


def open_stream_callback(_sender, _appdata, strm: stream.Stream):
    dpg.configure_item('_menu_open_stream', show=False)
    dpg.configure_item('_menu_close_stream', show=True)
    strm.enable(dpg.get_value('_baudrate_selector'))


def close_stream_callback(_sender, _appdata, strm: stream.Stream):
    dpg.configure_item('_menu_open_stream', show=True)
    dpg.configure_item('_menu_close_stream', show=False)
    strm.disable()
