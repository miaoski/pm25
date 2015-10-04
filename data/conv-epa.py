# -*- coding: utf8 -*-
# 去環保署測站下載 Excel 資料後，剪貼到 CSV 檔，再使用本程式轉換

import sys

f = open(sys.argv[1], 'r')
for line in f.readlines():
    xs = line.strip().split('\t')
    d = xs[0].replace('/', '-')
    for i in range(1, len(xs)):
        try:
            x = int(xs[i])
        except ValueError:
            x = -1
        print '%d' % (x, )
        # print '%s %02d:00:00,%d' % (d, i-1, x)
