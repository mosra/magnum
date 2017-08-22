var Module = {
    preRun: [],
    postRun: [],

    arguments: [],

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

/* Parse arguments, e.g. /app/?foo=bar&fizz&buzz=3 goes to the app as
   ['--foo', 'bar', '--fizz', '--buzz', '3'] */
var args = decodeURIComponent(window.location.search.substr(1)).trim().split('&');
for(var i = 0; i != args.length; ++i) {
    var j = args[i].indexOf('=');
    /* Key + value */
    if(j != -1) {
        Module.arguments.push('--' + args[i].substring(0, j));
        Module.arguments.push(args[i].substring(j + 1));

    /* Just key */
    } else Module.arguments.push('--' + args[i]);
}

Module.setStatus('Downloading...');

Module.canvas.addEventListener('contextmenu', function(event) {
    event.preventDefault();
}, true);
