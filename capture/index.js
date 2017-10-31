var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

app.get('/', function(req, res){
  res.sendFile(__dirname + '/index.html');
});

io.on('connection', function(socket){
	console.log('a user connected');


	socket.on('disconnect', function(){
		console.log('user disconnected');
	});
	socket.on('message', function(message){
		var x = message[0] + (message[1] << 8);
		var y = message[2] + (message[3] << 8);

		var msg = { mes: [socket.id, x, y] };
		// buffer.push(msg);
		// if (buffer.length > 15) buffer.shift();
		// client.broadcast(msg);
		console.log(msg);
	});
});

http.listen(3333, function(){
  console.log('listening on *:3333');
});