This project is made in order to learn linux programming, the basic concept and source code are inspired by muduo(https://github.com/chenshuo/muduo).

### High Performance Logger
    lock-free RingBuffer used in AsyncLogger.
### Reactor Model
    epoll(Level trigger) + nonblocking io
### Thread Model
    one EventLoop per thread.
### Timer
    timerfd + Timing Wheel
### TCPServer
    main thread(Acceptor) + thread pool(Worker threads)
### Idle Connections
    example/EchoServer
### Application Layer Keepalive
    todo
### TODO
    mysql, HTTP/1.1, DB connection, zero copy...
