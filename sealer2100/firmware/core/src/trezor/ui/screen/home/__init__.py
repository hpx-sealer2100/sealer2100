# 导入lvgl模块，这是一个用于嵌入式系统的图形库
import lvgl as lv
# 从当前包的home模块中导入HomeScreen类，HomeScreen类可能是一个表示主界面的屏幕类
from .home import HomeScreen

# 定义一个__all__列表，用于指定当使用from module import *时，哪些模块可以被导入
# 在这个例子中，只有HomeScreen类可以被导入
__all__ = ["HomeScreen"]
