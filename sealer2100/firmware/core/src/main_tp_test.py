import lvgl as lv
import lvgldrv as lcd  # type: ignore[Import "lvgldrv" could not be resolved]
from trezor import log


try:
    from trezor import io
    lv.init()
    disp_buf1 = lv.disp_draw_buf_t()
    buf1_1 = lcd.framebuffer(1)
    disp_buf1.init(buf1_1, None, len(buf1_1) // lv.color_t.__SIZE__)
    disp_drv = lv.disp_drv_t()
    disp_drv.init()
    disp_drv.draw_buf = disp_buf1
    disp_drv.flush_cb = lcd.flush
    disp_drv.hor_res = 480
    disp_drv.ver_res = 800
    disp_drv.register()

    indev_drv = lv.indev_drv_t()
    indev_drv.init()
    indev_drv.type = lv.INDEV_TYPE.POINTER
    indev_drv.read_cb = lcd.ts_read
    indev_drv.long_press_time = 2000
    indev_drv.register()

    # 创建一个屏幕对象
    scr = lv.obj()
    lv.scr_load(scr)

    # 记录上一个圆的坐标
    last_circle = None

    # 触摸事件回调函数
    def on_touch(event): #
        global last_circle

        # 获取触摸点坐标
        indev = lv.indev_get_act()
        point = lv.point_t()
        indev.get_point(point)
        log.debug("touch", "x=%d, y=%d", point.x, point.y)

        # 清除上一个圆
        if last_circle:
            last_circle.delete()

        # 创建一个新的圆
        new_circle = lv.obj(scr)
        new_circle.set_size(10, 10)
        new_circle.set_style_radius(5, 0)  # 设置圆角，使其变成圆形
        new_circle.set_pos(point.x - 5, point.y - 5)  # 居中对齐
        #红色
        new_circle.set_style_bg_color(lv.color_hex(0xFF0000), 0)

        # 记录当前圆
        last_circle = new_circle

    # 绑定触摸事件
    scr.add_event_cb(on_touch, lv.EVENT.PRESSED, None)






    while 1:
        lv.tick_inc(5)
        lv.task_handler()




except Exception as e:
    print(e)
