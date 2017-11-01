mongo --eval "var db = db.getSiblingDB('db'); db.cursor_positions.drop();";
./throughput 10 100 127.0.0.1 3000
