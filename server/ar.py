# -*- coding: utf8 -*-
import sqlite3
from flask import Flask, g
import uuid
import re

app = Flask(__name__)
DATABASE = './pm25.sq3'

re_uuid = re.compile(r'[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}')

def get_db():
    db = getattr(g, '_database', None)
    if db is None:
        db = g._database = sqlite3.connect(DATABASE)
    return db

@app.teardown_appcontext
def close_connection(exception):
    db = getattr(g, '_database', None)
    if db is not None:
        db.close()

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
        return 'OK', 200
	cur = get_db().cursor()
	cur.execute('insert into log values (?, datetime("now"), ?, ?, ?, ?)', (uuid, hum, tmp, mq9, dn7))
    return 'OK', 200


@app.route('/register')
def register():
    u = str(uuid.uuid4())
    return u, 200

if __name__ == "__main__":
    app.config['JSON_AS_ASCII'] = False     # JSON in UTF-8
    app.config['DEBUG'] = False
    app.run(host = '0.0.0.0', threaded=True, port=2080)
