var Module = {
    preRun: [],
    postRun: [],

    printErr: function(message) {
        console.error(Array.prototype.slice.call(arguments).join(' '));
    },

    print: function(message) {
        console.log(Array.prototype.slice.call(arguments).join(' '));
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
            Module.setStatus('Download complete');
            Module.setStatusDescription('');
        }
    }
};

Module.setStatus('Downloading...');

Module.canvas.addEventListener('contextmenu', function(event) {
    event.preventDefault();
}, true);
