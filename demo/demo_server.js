let WebSocketServer = require('uws').Server;
let wss = new WebSocketServer({ port: 8888 });
let sockets = [];
 
wss.on('connection', function(ws) {
    console.log ('Got client connection');
    sockets.push(ws);
});

function publish (data) {
    console.log("Sending " + JSON.stringify(data));
    sockets.forEach((socket) => { socket.send (JSON.stringify(data)); });
}

// Create a change stream on the collection, unwinding documents.

var MongoClient = require('mongodb').MongoClient;
// Connection URL
var url = 'mongodb://localhost:27017/db';

function restart_change_stream (db) {
    let coll = db.collection("cursor_positions");
    let cursor = coll.aggregate([
        {"$changeStream": {}},
        {"$project": { "fullDocument": 1, "_id": 1 } }
    ]);

    cursor.on("data", function(data) {
        publish (data);
    });

    cursor.on("end", function(data) {
        restart_change_stream ();
    })
}

// Use connect method to connect to the server
MongoClient.connect(url, function(err, db) {
    if (err) {
        console.log("Could not connect");
        return;
    }

    console.log("Connected successfully to server");

    restart_change_stream (db);
});
