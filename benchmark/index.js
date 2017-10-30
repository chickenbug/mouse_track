var WebSocketServer = require('uws').Server;
var wss = new WebSocketServer({ port: 3000 });
 
function onMessage(message) {
    this.send(message);
}
 
wss.on('connection', function(ws) {
    ws.on('message', onMessage);
});