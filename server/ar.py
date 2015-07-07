# -*- coding: utf8 -*-
import sqlite3
from flask import Flask, g
import uuid
import re

app = Flask(__name__)
DATABASE = '/home/ljm/www/pm25.sq3'

re_uuid = re.compile(r'[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}')

def connect_db():
    return sqlite3.connect(DATABASE)

@app.before_request
def before_request():
    g.db = connect_db()

@app.teardown_request
def teardown_request(exception):
    if hasattr(g, 'db'):
        g.db.close()

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
    app.run(host = '0.0.0.0', threaded=True, port=2080)
