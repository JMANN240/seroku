var zip = (a, b) => {
    return a.map(function(e, i) {
        return [e, b[i]];
    });
};
var xmlhttp = new XMLHttpRequest();
var xml;
xmlhttp.open('GET', `/apps`, true);
xmlhttp.send();
xmlhttp.onreadystatechange = () => {
    if (xmlhttp.readyState == 4) {
        if (xmlhttp.status == 200) {
            xml = xmlhttp.responseText;
            console.log(xml);
            var parser = new DOMParser();
            xmlDoc = parser.parseFromString(xml, 'text/xml');
            apps = xmlDoc.getElementsByTagName('app');
            var xmlhttp2 = new XMLHttpRequest();
            var appids;
            xmlhttp2.open('GET', `/appids`, true);
            xmlhttp2.send();
            xmlhttp2.onreadystatechange = () => {
                if (xmlhttp2.readyState == 4) {
                    if (xmlhttp2.status == 200) {
                        console.log(xmlhttp2.responseText);
                        appids = JSON.parse(xmlhttp2.responseText);
                        for (var [selected_app, select] of zip(appids, document.querySelectorAll('.channel-select'))) {
                            for (const app of apps) {
                                console.log(app.innerHTML, app.id);
                                const option = document.createElement('option');
                                option.value = app.id;
                                option.selected = app.id == selected_app;
                                const option_text = document.createTextNode(app.innerHTML.replace(/&amp;/g, '&'));
                                option.appendChild(option_text);
                                select.appendChild(option);
                            };
                            select.addEventListener('change', (e) => {
                                var xml;
                                xmlhttp.open('GET', `/app/${e.target.id.split('-')[1]-1}/${e.target.value}`, true);
                                xmlhttp.send();
                            });
                        };
                    }
                };
            };
        }
    };
};