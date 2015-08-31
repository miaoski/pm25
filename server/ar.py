# -*- coding: utf8 -*-
import sqlite3
from flask import Flask, g, send_from_directory
import uuid
import re
import requests
from threading import Timer

app = Flask(__name__, static_url_path='')
#DATABASE = '/home/ljm/www/pm25.sq3'
DATABASE = 'pm25.sq3'

TOKEN = open('token.txt').readline().strip()
UUID_GEO = {
        '54dc1e06-61ac-489f-a77f-0c3cd38a10d1' : [120.218, 23.006]
        }
global submit_thread
SUBMIT_INTERVAL=60

re_uuid = re.compile(r'[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}')


def now():
    import datetime
    return datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')

def timeshift(x):
    import datetime
    dt=datetime.datetime.utcnow() - datetime.timedelta(0,x)
    #return dt
    return dt.strftime('%Y-%m-%d %H:%M:00')

def connect_db():
    return sqlite3.connect(DATABASE)

@app.before_request
def before_request():
    g.db = connect_db()

@app.teardown_request
def teardown_request(exception):
    if hasattr(g, 'db'):
        g.db.close()

def get_agg_data():
    #conn = g.db
    conn = connect_db()
    c = conn.cursor()
    data={}
    s = 'SELECT id, strftime("%Y-%m-%dT%H:%M:00",timestamp), humidity, temperature, dn7val FROM log WHERE timestamp>="'+timeshift(SUBMIT_INTERVAL)+'" AND timestamp<"'+timeshift(0)+'"'
    print s
    try: 
        row = c.execute(s)
    except sqlite3.Error as e: 
        print e
        print 'sql:', s
        return False
    for r in row:
        #print r[0:2], r[2:]
        k=r[0]+':'+r[1]
        v=r[2:]
        if k not in data: data[k]=[0,0,0,0]
        for i in xrange(0,len(v)): data[k][i]+=v[i]
        data[k][-1]+=1
    r=[]
    for k in data:
        r.append(k.split(':', 1)+[v/data[k][-1] for v in data[k][:-1]])
    return r

def to_sheethub(x):
    import json
    url = 'https://sheethub.com/miaoski/pm25/insert?access_token=%s' % TOKEN
    headers = {"Content-Type": "application/x-www-form-urlencoded"}
    d = 'data=' + json.dumps(x)
    r = requests.post(url, data=d, headers=headers, verify=False)
    if r.status_code != requests.codes.ok:
        print 'Failed to submit', r.status_code, x[0][0], x[0][1]

def task_sheethub():
    row=get_agg_data()
    submit_thread=Timer(SUBMIT_INTERVAL, task_sheethub)
    submit_thread.daemon=True
    submit_thread.start()
    data=[]
    if row: 
        data=[r+UUID_GEO[r[0]] for r in row if r[0] in UUID_GEO]
    print now(), data
    if len(data) > 0:
        to_sheethub(data)

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
    return 'OK', 200



@app.route('/register')
def register():
    u = str(uuid.uuid4())
    return u, 200

if __name__ == "__main__":
    app.config['JSON_AS_ASCII'] = False     # JSON in UTF-8
    app.config['DEBUG'] = False
    task_sheethub()
    app.run(host = '0.0.0.0', threaded=True, port=2080)
