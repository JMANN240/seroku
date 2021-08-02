#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <uri/UriBraces.h>

ESP8266WebServer server(80);

IPAddress ssdp_addr(239,255,255,250);
IPAddress tv_addr;
WiFiClient client;
HTTPClient http;
WiFiUDP udp;
char result[16];              // Buffer big enough for 7-character float
char packetBuffer[255];       // buffer for incoming data
int app_ids[] = {12, 2285, 19977, 143105, 837, 593099};

void handleRoot() {
  char res[2048];
  char* pos = res;
  sprintf(pos, "<!DOCTYPE html><html> <head> <meta name='viewport' content='width=device-width'> <meta charset='UTF-8' /> <title>Roku</title> <script defer src='/script'></script> <link rel='stylesheet' type='text/css' href='/style'> </head> <body> <button class='key top' id='Home'>Home</button> <button class='key top' id='Up'>Up</button> <button class='key top' id='PowerOff'>Off</button> <button class='key top' id='PowerOn'>On</button> <button class='key top' id='Left'>Left</button> <button class='key top' id='Select'>OK</button> <button class='key top' id='Right'>Right</button> <button class='key top' id='VolumeUp'>+</button> <button class='key top' id='Back'>Back</button> <button class='key top' id='Down'>Down</button> <button class='key top' id='VolumeMute'>Mute</button> <button class='key top' id='VolumeDown'>-</button> <button class='key bottom' id='Rew'>Rewind</button> <button class='key bottom' id='Play'>Play/Pause</button> <button class='key bottom' id='Fwd'>Fast Forward</button> <button class='ch bottom' id='ch-1'></button> <button class='ch bottom' id='ch-2'></button> <button class='ch bottom' id='ch-3'></button> <button class='ch bottom' id='ch-4'></button> <button class='ch bottom' id='ch-5'></button> <button class='ch bottom' id='ch-6'></button> </body></html>");
  server.send(200, "text/html", res);
}

void handleScript() {
  char res[2048];
  char* pos = res;
  sprintf(pos, "document.querySelector('body').style.height=`${window.innerHeight}px`;var tv_addr='http://%s:8060';for(const e of document.querySelectorAll('.key'))e.addEventListener('click',e=>{var t=new XMLHttpRequest;t.open('POST',`${tv_addr}/keypress/${e.target.id}`,!0),t.send()});var xml,xmlhttp=new XMLHttpRequest;xmlhttp.open('GET','/apps',!0),xmlhttp.send(),xmlhttp.onreadystatechange=(()=>{if(4==xmlhttp.readyState){xml=xmlhttp.responseText;var e=new DOMParser;xmlDoc=e.parseFromString(xml,'text/xml'),apps=xmlDoc.getElementsByTagName('app'),xmlhttp.open('GET','/appids',!0),xmlhttp.send(),xmlhttp.onreadystatechange=(()=>{if(4==xmlhttp.readyState){appids=JSON.parse(xmlhttp.responseText);for(const[t,n]of appids.entries()){var e=document.querySelector(`#ch-${t+1}`);e.innerHTML=xmlDoc.getElementById(`${n}`).innerHTML,e.value=n,e.addEventListener('click',e=>{var t=new XMLHttpRequest;t.open('POST',`${tv_addr}/launch/${e.target.value}`,!0),t.send()})}}})}});", tv_addr.toString().c_str());
  server.send(200, "text/javascript", res);
}

void handleStyle() {
  char res[1024];
  char* pos = res;
  sprintf(pos, "html { touch-action: manipulation;} body { background-color: #202020; margin: 0px; padding: 0px; display: grid; grid-template-rows: repeat(6, 1fr); grid-template-columns: repeat(12, 1fr); height: 100vh; width: 100vw;}button { margin: 2vmin; font-size: 5vmin; border: none; color: #ffffff; border-radius: 2vmin; background-color: #202020; -webkit-box-shadow: 0 4px 0 0 #101010,0 4px 10px 2px #000000; box-shadow: 0 4px 0 0 #101010,0 4px 10px 2px #000000; transition-duration: 0.1s;}button:active { -webkit-box-shadow: 0 0 0 0 #101010, 0 0 0 0 #000000; box-shadow: 0 0 0 0 #101010, 0 0 0 0 #000000; transform: translateY(4px);}button.top { grid-column-end: span 3;}button.bottom { grid-column-end: span 4;}");
  server.send(200, "text/css", res);
}

void handleSettings() {
  char res[2048];
  char* pos = res;
  sprintf(pos, "<!DOCTYPE html><html> <head> <meta name='viewport' content='width=device-width'> <meta charset='UTF-8' /> <title>Roku</title> </head> <body> <h1>ESP IP: %s</h1> <select class='channel-select' id='channel-1'> </select> <select class='channel-select' id='channel-2'> </select> <select class='channel-select' id='channel-3'> </select> <select class='channel-select' id='channel-4'> </select> <select class='channel-select' id='channel-5'> </select> <select class='channel-select' id='channel-6'> </select> <script> var zip = (a, b) => { return a.map(function(e, i) { return [e, b[i]]; }); }; var xmlhttp = new XMLHttpRequest(); var xml; xmlhttp.open('GET', `/apps`, true); xmlhttp.send(); xmlhttp.onreadystatechange = () => { if (xmlhttp.readyState == 4) { if (xmlhttp.status == 200) { xml = xmlhttp.responseText; console.log(xml); var parser = new DOMParser(); xmlDoc = parser.parseFromString(xml, 'text/xml'); apps = xmlDoc.getElementsByTagName('app'); var xmlhttp2 = new XMLHttpRequest(); var appids; xmlhttp2.open('GET', `/appids`, true); xmlhttp2.send(); xmlhttp2.onreadystatechange = () => { if (xmlhttp2.readyState == 4) { if (xmlhttp2.status == 200) { console.log(xmlhttp2.responseText); appids = JSON.parse(xmlhttp2.responseText); for (var [selected_app, select] of zip(appids, document.querySelectorAll('.channel-select'))) { for (const app of apps) { console.log(app.innerHTML, app.id); const option = document.createElement('option'); option.value = app.id; option.selected = app.id == selected_app; const option_text = document.createTextNode(app.innerHTML.replace(/&amp;/g, '&')); option.appendChild(option_text); select.appendChild(option); }; select.addEventListener('change', (e) => { var xml; xmlhttp.open('GET', `/app/${e.target.id.split('-')[1]-1}/${e.target.value}`, true); xmlhttp.send(); }); }; } else { console.log('Error: ' + xmlhttp2.status); }; }; }; } else { console.log('Error: ' + xmlhttp.status); }; }; }; </script> </body></html>",WiFi.localIP().toString().c_str(), tv_addr.toString().c_str());
  server.send(200, "text/html", res);
}

void handleSettingsScript() {
  char res[2048];
  char* pos = res;
  sprintf(pos, "var zip = (a, b) => { return a.map(function(e, i) { return [e, b[i]]; });};var xmlhttp = new XMLHttpRequest();var xml;xmlhttp.open('GET', `/apps`, true);xmlhttp.send();xmlhttp.onreadystatechange = () => { if (xmlhttp.readyState == 4) { if (xmlhttp.status == 200) { xml = xmlhttp.responseText; console.log(xml); var parser = new DOMParser(); xmlDoc = parser.parseFromString(xml, 'text/xml'); apps = xmlDoc.getElementsByTagName('app'); var xmlhttp2 = new XMLHttpRequest(); var appids; xmlhttp2.open('GET', `/appids`, true); xmlhttp2.send(); xmlhttp2.onreadystatechange = () => { if (xmlhttp2.readyState == 4) { if (xmlhttp2.status == 200) { console.log(xmlhttp2.responseText); appids = JSON.parse(xmlhttp2.responseText); for (var [selected_app, select] of zip(appids, document.querySelectorAll('.channel-select'))) { for (const app of apps) { console.log(app.innerHTML, app.id); const option = document.createElement('option'); option.value = app.id; option.selected = app.id == selected_app; const option_text = document.createTextNode(app.innerHTML.replace(/&amp;/g, '&')); option.appendChild(option_text); select.appendChild(option); }; select.addEventListener('change', (e) => { var xml; xmlhttp.open('GET', `/app/${e.target.id.split('-')[1]-1}/${e.target.value}`, true); xmlhttp.send(); }); }; } }; }; } };};");
  server.send(200, "text/javascript", res);
}

void apps() {
  char res[2048];
  char* pos = res;
  pos += sprintf(pos, "http://%s:8060/query/apps", tv_addr.toString().c_str());
  Serial.println(res);
  http.begin(client, res);
  int http_code = http.GET();
  Serial.println(http_code);
  if (http_code > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] POST... code: %d\n", http_code);

    // file found at server
    if (http_code == HTTP_CODE_OK) {
      const String& payload = http.getString();
      Serial.println("received payload:\n<<");
      Serial.println(payload);
      Serial.println(">>");
      server.send(200, "text/xml", payload);
    }
  } else {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(http_code).c_str());
  }
  http.end();
}

void app() {
  app_ids[server.pathArg(0).toInt()] = int(server.pathArg(1).toInt());
  Serial.println(server.pathArg(0));
  Serial.println(server.pathArg(1));
  server.send(200, "text/html", "");
}

void appids() {
  char res[256];
  char* pos = res;
  pos += sprintf(pos, "[");
  for (int i = 0; i < 6; ++i) {
    pos += sprintf(pos, "%i", app_ids[i]);
    if (i < 5) {
      pos += sprintf(pos, ", ");
    }
  }
  pos += sprintf(pos, "]");
  server.send(200, "text/plain", res);
}

void setup() {
  // put your setup code here, to run once:
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  WiFiManager wm;
  wm.autoConnect();
  udp.begin(1900);
  int packetSize = 0;
  while (!packetSize) {
    udp.beginPacketMulticast(ssdp_addr, 1900, WiFi.localIP(), 8);
    udp.write("M-SEARCH * HTTP/1.1\nHost: 239.255.255.250:1900\nMan: ssdp:discover\nST: roku:ecp\n");
    udp.endPacket();
    Serial.println("Sending UDP packet");
    packetSize = udp.parsePacket();
    if (packetSize) {
      tv_addr = udp.remoteIP();
      Serial.print("Received packet of size ");
      Serial.println(packetSize);
      Serial.print("From ");
      IPAddress remoteIp = udp.remoteIP();
      Serial.print(remoteIp);
      Serial.print(", port ");
      Serial.println(udp.remotePort());

      // read the packet into packetBufffer
      int len = udp.read(packetBuffer, 255);
      if (len > 0) {
        packetBuffer[len] = 0;
      }
      Serial.print("Contents:");
      Serial.println(packetBuffer);
      break;
    }
    delay(1000);
  }

  Serial.print("tv_addr = ");
  Serial.println(tv_addr);
  
  MDNS.begin("roku");
  server.on("/", handleRoot);
  server.on("/settings", handleSettings);
  server.on("/script", handleScript);
  server.on("/style", handleStyle);
  server.on("/settingsscript", handleSettingsScript);
  server.on("/appids", appids);
  server.on(UriBraces("/app/{}/{}"), app);
  server.on("/apps", apps);
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  MDNS.update();
}