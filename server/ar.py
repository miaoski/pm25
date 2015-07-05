from flask import Flask
import uuid

app = Flask(__name__)

@app.route('/id/<uuid>/hum/<hum>/tmp/<tmp>/mq9/<mq9>/dn7/<dn7>')
def submit_value(uuid, hum, tmp, mq9, dn7):
	hum = float(hum)
	tmp = float(tmp)
	mq9 = float(mq9)
	dn7 = float(dn7)
	print "Got params", uuid, hum, tmp, mq9, dn7
	if len(uuid) != 36:
		print 'Invalid UUID!'
	return 'OK', 200

@app.route('/register')
def register():
	u = str(uuid.uuid4())
	return u, 200

if __name__ == "__main__":
	app.config['JSON_AS_ASCII'] = False     # JSON in UTF-8
	app.config['DEBUG'] = True
	app.run(host = '0.0.0.0', threaded=True, port=2080)
