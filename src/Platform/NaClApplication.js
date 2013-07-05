function setStatus(message) {
    var status = document.getElementById('status');
    if(status) status.innerHTML = message;
}

function setStatusDescription(message) {
    var statusDescription = document.getElementById('statusDescription');
    if(statusDescription) statusDescription.innerHTML = message;
}

var listener = document.getElementById('listener');

listener.addEventListener('loadstart', function() {
    setStatus('Loading...');
}, true);

listener.addEventListener('progress', function(event) {
    setStatus('Downloading...');

    /* Show progress */
    if(event.lengthComputable && event.total > 0)
        setStatusDescription(Math.round(event.loaded*100/event.total) + '% of '
            + Math.round(event.total/1024) + ' kB');

    /* Unknown total size */
    else setStatusDescription(Math.round(event.loaded/1024) + ' kB');

}, true);

listener.addEventListener('error', function() {
    setStatus('Loading failed');
    setStatusDescription(document.getElementById('module').lastError + '<br />Check JavaScript console for more information.');
}, true);

listener.addEventListener('abort', function() {
    setStatus('Loading aborted');
    setStatusDescription('');
}, true);

listener.addEventListener('load', function() {
    setStatus('');
    setStatusDescription('');
}, true);
