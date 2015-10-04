# -*- coding: utf8 -*-
# 去環保署測站下載 Excel 資料後，剪貼到 CSV 檔，再使用本程式轉換

import sys
import datetime

f = open(sys.argv[1], 'r')
for line in f.readlines():
    xs = line.strip().split('\t')
    d = datetime.datetime.strptime(xs[0], '%Y/%m/%d')
    for i in range(1, len(xs)):
        dt = d + datetime.timedelta(hours = (i-1-8))    # to UTC
        try:
            x = int(xs[i])
        except ValueError:
            x = -1
        # print '%d' % (x, )
        print '%s,%d' % (dt.strftime('%Y/%m/%d %H:%M'), x)
