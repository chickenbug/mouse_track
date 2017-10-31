- rudimentary benchmark on local machine shows packet size of 4096 bytes is good.
Q- we need a replica set, Atlas M0 won't cut it. What do?
Q- how many websockets can we support? How do we analyze performance?
Q- can we increase performance by wrapping inserts around uv async functions?

Optimizations:
- adaptive sampling rate with congestion control
- custom memory allocator in C to prevent calls to malloc/free for each insert
- call mongoc_command directly to prevent additional bson_t's from being created
- cheat by pre-allocating a command per user, and *directly* modifying the underlying data with the coordinates.
- run through valgrind to check for leaks

Today:
- make a demo with changestreams + Node server
- [HK] send data at a fixed sampling rate, and interpolate on the client side
- [ME] get metric baselines
- optimize to improve those baselines
