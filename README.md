### About

Due to the "Closed World" assumption of DMTCP (Distributed MultiThreaded CheckPointing), it cannot monitor a connection made by an application to external services (for e.g. databases). Therefore it marks the connection socket as an "external socket" before checkpointing and recreates it as a "dead socket" afterwards, effectively disrupting the connection to the external service.

This plugin safely disconnects an application from a database (MySQL in this case) before checkpointing and automatically reconnects on resume/restart without the application being aware of it.

### Dependencies

- [DMTCP](http://dmtcp.sourceforge.net/)
- MySQL
- MySQL client library

### Compiling

- Configure the shell variable DMTCP_ROOT to point to the DMTCP installation

- Compiling the plugin
```
    gcc -g -shared -fPIC -I$DMTCP_ROOT/include plugin.c -o plugin.so -std=c99 `mysql_config --cflags --libs`
```
- Compile example code
```
    gcc examples/c_mysql3.c -o c_mysql3 -std=c99 `mysql_config --cflags –libs`
```
### Launching test application

    $DMTCP_ROOT/bin/dmtcp_launch --with-plugin plugin.so ./c_mysql3 <host> <username> <password> <database> <port>
