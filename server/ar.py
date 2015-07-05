# -*- coding: utf8 -*-
from flask import Flask
import uuid
import re

app = Flask(__name__)

re_uuid = re.compile(r'[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}')

@app.route('/id/<uuid>/hum/<hum>/tmp/<tmp>/mq9/<mq9>/dn7/<dn7>')
def submit_value(uuid, hum, tmp, mq9, dn7):
	"不論如何都回應 OK, 200 免得有人來 try UUID"
	try:
		hum = float(hum)
		tmp = float(tmp)
		mq9 = float(mq9)
		dn7 = float(dn7)
		assert(re_uuid.match(uuid) is not None)
	except:
		return 'OK', 200
	print "Got params", uuid, hum, tmp, mq9, dn7
	print "Save to DB"
	return 'OK', 200


@app.route('/register')
def register():
	u = str(uuid.uuid4())
	return u, 200

if __name__ == "__main__":
	app.config['JSON_AS_ASCII'] = False     # JSON in UTF-8
	app.config['DEBUG'] = False
	app.run(host = '0.0.0.0', threaded=True, port=2080)
