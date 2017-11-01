let WebSocketServer = require('uws').Server;
let wss = new WebSocketServer({ port: 3000 });
let test_sockets = [];
let test_bytes_expecting = 0;
const MSG_MASTER = 0x01;
const MSG_CLIENT_TEST = 0x02;
const MSG_SERVER_ACK = 0x03;
const MSG_SERVER_ALL_RECV = 0x04;
let db = null;

wss.on('connection', function(socket){
    console.log('a user connected');

    socket.on('message', function(arrayBuffer){
        var message = new Uint8Array(arrayBuffer);

        if (message.length == 5 && message[0] == MSG_CLIENT_TEST) {
            // Add sockets.
            test_sockets.push (socket);

            // Next four bytes are expected total bytes to receive in the test.
            test_bytes_expecting = message[1] + (message[2] << 8) + (message[3] << 16) + (message[4] << 24);
            console.log("Got MSG_CLIENT_TEST with test_bytes_expecting=" + test_bytes_expecting);
            in_test = true;

            // Acknowledge test starts
            socket.send (Uint8Array.from([MSG_SERVER_ACK]));

            return;
        }

        record_data(message);

        if (in_test) {
            test_bytes_expecting -= message.length;

            if (test_bytes_expecting <= 0) {
                console.log("end of test, broadcasting ALL_RECV");
                test_sockets.forEach((socket) => { socket.send(Uint8Array.from([ MSG_SERVER_ALL_RECV ]))});
                test_sockets = [];
                in_test = false;
                test_bytes_expecting = 0;
            }
        }
    });
});
 
wss.on('connection', function(ws) {
    console.log ('Got client connection');
});

function publish (data) {
    console.log("Sending " + JSON.stringify(data));
    sockets.forEach((socket) => { socket.send (JSON.stringify(data)); });
}

var MongoClient = require('mongodb').MongoClient;
// Connection URL
var uri = 'mongodb://localhost:27017/db';

// Use connect method to connect to the server
MongoClient.connect(uri, function(err, mongodb) {
    if (err) {
        console.log("Could not connect");
        return;
    }
    db = mongodb;
});

function record_data (message) {
    // Create the document from the message data (ignore user id for now).
    let d = [];
    for (let i = 0; i < message.length; i += 4) {
        let x = message[i] + (message[i+1] << 8);
        let y = message[i+2] + (message[i+3] << 8);
        d.push({x: x, y: y});
    }
    let coll = db.collection("cursor_positions");
    coll.writeConcern.w = 0;
    coll.insertOne({d: d})
}
