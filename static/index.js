document.querySelector('body').style.height = `${window.innerHeight}px`;
var tv_addr = 'http://%s:8060';
for (const key of document.querySelectorAll('.key')) {
    key.addEventListener('click', (e) => {
        var xmlhttp = new XMLHttpRequest();
        xmlhttp.open('POST', `${tv_addr}/keypress/${e.target.id}`, true);
        xmlhttp.send();
    });
}

var xmlhttp = new XMLHttpRequest();
var xml;
xmlhttp.open('GET', `/apps`, true);
xmlhttp.send();
xmlhttp.onreadystatechange = () => {
    if (xmlhttp.readyState == 4) {
    xml = xmlhttp.responseText;
    var parser = new DOMParser();
    xmlDoc = parser.parseFromString(xml, 'text/xml');
    apps = xmlDoc.getElementsByTagName('app');
    xmlhttp.open('GET', `/appids`, true);
    xmlhttp.send();
    xmlhttp.onreadystatechange = () => {
        if (xmlhttp.readyState == 4) {
        appids = JSON.parse(xmlhttp.responseText);
        for (const [index, app_id] of appids.entries()) {
            var ch = document.querySelector(`#ch-${index+1}`);
            ch.innerHTML = xmlDoc.getElementById(`${app_id}`).innerHTML;
            ch.value = app_id;
            ch.addEventListener('click', (e) => {
            var xmlhttp = new XMLHttpRequest();
            xmlhttp.open('POST', `${tv_addr}/launch/${e.target.value}`, true);
            xmlhttp.send();
            });
        }
        };
    };
    };
};