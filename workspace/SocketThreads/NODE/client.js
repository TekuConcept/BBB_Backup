var net = require('net');

var client = new net.Socket();
var clienta = new net.Socket();

// connect to host server and fetch child server info
client.connect(5041, '127.0.0.1', function() {
	console.log('CLIENT: Connected');
	client.write('stuff\n');
	client.read();
});

client.on('data', function(data) {
	console.log('CLIENT: Received: ' + data);
	clienta.connect(parseInt(data), '127.0.0.1', function() {
		clienta.write('hi_friend_from_client');
		clienta.destroy();	
	});
	client.destroy(); // kill client after server's response
});

client.on('close', function() {
	console.log('CLIENT: Connection closed');
});

client.on('error', function(err) {
	console.log('CLIENT: An error occurred');
	console.log(err);
});


// connect to child server
clienta.on('data', function(data) {
	console.log('N-CLIENT-A: Received: ' + data);	
});

clienta.on('close', function() {
	console.log('N-CLIENT-A: Connection closed');
});

clienta.on('error', function(err) {
	console.log('N-CLIENT-A: An error occurred');
	console.log(err);
});