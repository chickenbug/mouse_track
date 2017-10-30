# Current Tasks
- quick benchmark NodeJS uWebsocket vs uWebsocket C++
    - find optimal chunk size to send over websockets [done]
    4096 bytes seems ok.
- capture cursor positions on the front-end [done]
- see how many open websockets we can support on a single digital ocean server
- benchmark NodeJS driver vs libmongoc on a simple insert workload
- couple issues:
    - we need a replica set, Atlas M0 won't cut it.