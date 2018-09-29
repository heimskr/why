# <a name="intro"></a>Net Extension

This extension provides trap operations allowing applications to communicate via sockets. For now, the C VM will implement TCP over IPv4. In the future, the JS VM might implement WebSockets support and the C VM might implement UDP support.

Only one connection is allowed at a time.

# <a name="net-t"></a>Traps

### <a name="op-xn-init"></a>Net Init (`xn_init`)
Syntax: `<xn init>`  
Function value: `000010000000`

Sets up the program for socket communication.

### <a name="op-xn-connect"></a>Net Connect (`xn_connect`)
Syntax: `<xn connect rs rt>`  
Function value: `000010000001`

Connects to the IP address stored in the string beginning at the address stored in `rs` on the port stored in `rt`.


### <a name="op-xn-send"></a>Net Send (`xn_send`)
Syntax: `<xn send rs>`  
Function value: `000010000010`

Sends the string starting at the address stored in `rs` through the socket.

### <a name="op-xn-recv"></a>Net Receive (`xn_recv`)
Syntax: `<xn recv rd rs>`
Function value: `000010000011`

Receives `rs` bytes from the socket and stores them as a string starting at the address stored in `rd`.
