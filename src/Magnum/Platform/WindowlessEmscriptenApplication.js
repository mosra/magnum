var Module = {
    preRun: [],
    postRun: [],

    printErr: function(message) {
        var log = document.getElementById('log');
        log.innerHTML += Array.prototype.slice.call(arguments).join(' ') + '\n';
    },

    print: function(message) {
        var log = document.getElementById('log');
        log.innerHTML += Array.prototype.slice.call(arguments).join(' ') + '\n';
    },

    canvas: document.getElementById('module'),

    setStatus: function(message) {
        var status = document.getElementById('status');
        if(status) status.innerHTML = message;
    },

    setStatusDescription: function(message) {
        var statusDescription = document.getElementById('statusDescription');
        if(statusDescription) statusDescription.innerHTML = message;
    },

    totalDependencies: 0,

    monitorRunDependencies: function(left) {
        this.totalDependencies = Math.max(this.totalDependencies, left);

        if(left) {
            Module.setStatus('Downloading...');
            Module.setStatusDescription((this.totalDependencies - left) + '/' + this.totalDependencies);
        } else {
            Module.setStatus('');
            Module.setStatusDescription('');
        }
    }
};

Module.setStatus('Downloading...');

Module.canvas.addEventListener('contextmenu', function(event) {
    event.preventDefault();
}, true);
