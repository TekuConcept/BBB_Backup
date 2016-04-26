var net = require('net');

var client = new net.Socket();
client.connect(parseInt(process.argv[2]), '127.0.0.1', function() {
	console.log('CLIENT: Connected');
	// client.write('stuff\n');
	client.write('exit\n');
	// client.read();
});

client.on('data', function(data) {
	console.log('CLIENT: Received: ' + data);
	client.destroy(); // kill client after server's response
});

client.on('close', function() {
	console.log('CLIENT: Connection closed');
});

client.on('error', function(err) {
	console.log('CLIENT: An error occurred');
	console.log(err);
});
