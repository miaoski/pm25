import sqlite3
import datetime

def parse_time(t):
    return datetime.datetime.strptime(t, '%Y/%m/%d %H:%M')


tainan = {}
for l in open('tainan.csv', 'r').readlines():
    x = l.strip().split(',')
    tainan[parse_time(x[0])] = x[1]


thirty = datetime.timedelta(minutes = 30)

db = sqlite3.connect('pm25.sq3')
cur = db.cursor()

time_format = '%Y-%m-%d %H:%M:00'

uuid = '54dc1e06-61ac-489f-a77f-0c3cd38a10d1'
ts = tainan.keys()
ts.sort()
for t in ts:
    cur.execute('select avg(dn7val) from log where timestamp < ? and timestamp >= ? and id=?',
        ((t + thirty).strftime(time_format),
         (t - thirty).strftime(time_format),
         uuid))
    rows = cur.fetchall()
    if len(rows) == 0:
        print ','.join((uuid, t.strftime(time_format), tainan[t], "-1"))
    else:
        for row in rows:
            print ','.join((uuid, t.strftime(time_format), tainan[t], str(row[0])))
