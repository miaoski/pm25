# -*- coding: utf8 -*-
import sqlite3
from flask import Flask, g, send_from_directory
import uuid
import re
import requests

app = Flask(__name__, static_url_path='')
DATABASE = '/home/ljm/www/pm25.sq3'

TOKEN = open('token.txt').readline().strip()
UUID_GEO = {
        '54dc1e06-61ac-489f-a77f-0c3cd38a10d1' : [120.218, 23.006]
        }

re_uuid = re.compile(r'[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}')


def now():
    import datetime
    return datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')


def connect_db():
    return sqlite3.connect(DATABASE)

@app.before_request
def before_request():
    g.db = connect_db()

@app.teardown_request
def teardown_request(exception):
    if hasattr(g, 'db'):
        g.db.close()

def to_sheethub(x):
    import json
    url = 'https://sheethub.com/miaoski/pm25/insert?access_token=%s' % TOKEN
    headers = {"Content-Type": "application/x-www-form-urlencoded"}
    d = 'data=' + json.dumps(x)
    r = requests.post(url, data=d, headers=headers, verify=False)
    if r.status_code != requests.codes.ok:
        print 'Failed to submit', r.status_code, x[0][0], x[0][1]

@app.route('/pm25.sq3')
def send_pm25_sq3():
    return send_from_directory('.', 'pm25.sq3')


@app.route('/id/<uuid>/hum/<hum>/tmp/<tmp>/mq9/<mq9>/dn7/<dn7>')
def submit_value(uuid, hum, tmp, mq9, dn7):
    "Return 200 OK to avoid UUID hijack"
    try:
        hum = float(hum)
        tmp = float(tmp)
        mq9 = float(mq9)
        dn7 = float(dn7)
        assert(re_uuid.match(uuid) is not None)
    except:
        raise
        return 'OK', 200
    conn = g.db
    c = conn.cursor()
    c.execute('insert into log values (?, datetime("now"), ?, ?, ?, ?)', (uuid, hum, tmp, mq9, dn7))
    conn.commit()
    c.close()
    geoxy = UUID_GEO.get(uuid, [-1, -1])
    if geoxy[0] != -1:
        x = [[uuid, now(), hum, tmp, dn7, geoxy[0], geoxy[1]]]
        print x
        to_sheethub(x)
    return 'OK', 200



@app.route('/register')
def register():
    u = str(uuid.uuid4())
    return u, 200

if __name__ == "__main__":
    app.config['JSON_AS_ASCII'] = False     # JSON in UTF-8
    app.config['DEBUG'] = False
    app.run(host = '0.0.0.0', threaded=True, port=2080)
