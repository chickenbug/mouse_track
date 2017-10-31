var app = require('express')();
var http = require('http').Server(app);
let WebSocketServer = require('uws').Server;
let wss = new WebSocketServer({ port: 3000 });

app.get('/', function(req, res){
  res.sendFile(__dirname + '/index.html');
});

wss.on('connection', function(socket){
	console.log('a user connected');


	socket.on('disconnect', function(){
		console.log('user disconnected');
	});
	socket.on('message', function(arrayBuffer){
		var message = new Uint8Array(arrayBuffer);

		var x = message[0] + (message[1] << 8);
		var y = message[2] + (message[3] << 8);

		var msg = { mes: [ x, y] };
		// buffer.push(msg);
		// if (buffer.length > 15) buffer.shift();
		// client.broadcast(msg);
		console.log(msg);
	});
});