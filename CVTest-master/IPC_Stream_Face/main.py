# -*- coding:utf-8 -*-
#
# from monitor import start_monitor
#
#
# if __name__ == '__main__':
#     start_monitor([10, 11], 9009)

from monitor_http import app

if __name__ == '__main__':
    app.run("0.0.0.0", 18888)