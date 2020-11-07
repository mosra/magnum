Module.canvas = document.getElementById('canvas1');
Module.status = document.getElementById('status1');
Module.statusDescription = document.getElementById('status-description1');

Module.keyboardListeningElement = Module.canvas;
Module.canvas.addEventListener('mousedown', function(event) {
    event.target.focus();
});
