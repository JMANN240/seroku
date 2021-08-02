from flask import Flask, request, make_response, jsonify, send_from_directory
from flask.templating import render_template

app = Flask(__name__)

@app.route('/', methods=['M-SEARCH', 'GET'])
def index():
    if request.method == 'GET':
        return render_template('roku.html')

    if request.method == 'M-SEARCH':
        if request.headers.get('Man') == "\"ssdp:discover\"" and request.headers.get('ST') == "roku:ecp":
            res = make_response("200")
            res.headers['Cache-Control'] = "max-age=3600"
            res.headers['ST'] = "roku:ecp"
            res.headers['Location'] = "http://192.168.0.3:1900"
            res.headers['USN'] = "uuid:roku:ecp:SOMEUSN"
            print(res.headers)
            return res

@app.route('/script')
def script():
    return send_from_directory('static', 'index.js')

@app.route('/style')
def style():
    return send_from_directory('static', 'styles.css')

@app.route('/settingsscript')
def settingsscript():
    return send_from_directory('static', 'settings.js')

@app.route('/settings')
def settings():
    return render_template('roku-settings.html')

@app.route('/apps')
def apps():
    res = make_response('''<?xml version="1.0" encoding="UTF-8" ?>
        <apps>
            <app id="12" type="appl" version="5.0.98079419">Netflix</app>
            <app id="2285" type="appl" version="6.45.2">Hulu</app>
            <app id="19977" type="appl" version="2.7.16">Spotify Music</app>
            <app id="143105" type="appl" version="1.2.70">VRV</app>
            <app id="837" type="appl" version="2.21.94005076">YouTube</app>
            <app id="61322" type="appl" version="50.12.30">HBO Max</app>
            <app id="291097" type="appl" version="1.14.2021062101">Disney Plus</app>
            <app id="151908" type="appl" version="4.2.3">The Roku Channel</app>
            <app id="41468" type="appl" version="2.16.7">Tubi - Free Movies &amp; TV</app>
            <app id="tvinput.hdmi1" type="tvin" version="1.0.0">HDMI 1 (ARC)</app>
            <app id="tvinput.hdmi2" type="tvin" version="1.0.0">HDMI 2</app>
            <app id="tvinput.hdmi3" type="tvin" version="1.0.0">HDMI 3</app>
            <app id="tvinput.cvbs" type="tvin" version="1.0.0">AV</app>
            <app id="tvinput.dtv" type="tvin" version="1.0.0">Live TV</app>
            <app id="27536" type="appl" version="5.10.19">CBS News</app>
            <app id="54065" type="appl" version="3.3.4">ABC News Live</app>
            <app id="593099" type="appl" version="2.7.62">Peacock TV</app>
            <app id="50025" type="appl" version="1.0.100">Google Play Movies &amp; TV</app>
        </apps>''')
    res.headers['Content-Type'] = 'text/xml'
    return res

@app.route('/appids')
def appids():
    return "[12, 2285, 19977, 143105, 837, 593099]"

@app.route('/keypress/<key>', methods=['POST'])
def keypress(key):
    print('pressed ' + key)
    return "200"

@app.route('/launch/<appid>', methods=['POST'])
def launch(appid):
    print('launched ' + appid)
    return "200"

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=1900, debug=True)