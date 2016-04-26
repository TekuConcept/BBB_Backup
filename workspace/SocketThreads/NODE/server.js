var net = require('net');

var server = net.createServer(function(socket) {
	socket.pipe(socket);

	socket.on('connect', function() {
		console.log('Client connected: ' + socket.localport);
	});

	socket.on('close', function() {
		console.log('Server closing');
	});

	socket.on('error', function(err) {
		console.log('An error occured');
	});
});

server.listen(5040, '127.0.0.1');