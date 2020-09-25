# OpenPMD Protocol

## Data Types

Note: all data is sent over the network in little endian format.

| Name       | Size              | Description                                                  |
| ---------- | ----------------- | ------------------------------------------------------------ |
| uint8      | 1 byte            | An unsigned 8-bit integer with a range of 0 to 255           |
| int8       | 1 byte            | A signed 8-bit integer with a range of -128 to 127           |
| uint16     | 2 bytes           | An unsigned 16-bit integer with a range of 0 to 65535        |
| int16      | 2 bytes           | A signed 16-bit integer with a range of -32768 to 32767      |
| uint32     | 4 bytes           | An unsigned 32-bit integer with a range of 0 to 4294967296   |
| int32      | 4 bytes           | A signed 32-bit integer with a range of -2147483648 to 2147483647 |
| uint64     | 8 bytes           | An unsigned integer with a range of 0 to 2^64-1              |
| int64      | 8 bytes           | A signed integer with a range of -2^63 to 2^63-1             |
| float      | 4 bytes           | An IEEE-754 single precision floating point number           |
| double     | 8 bytes           | An IEEE-754 double precision floating point number           |
| string     | 2 to 65537 bytes  | A length-prefixed (uint16) UTF-8 string                      |
| binary     | 2 to 65537 bytes  | Length-prefixed (uint16) binary data                         |
| longstring | 4 to 2^32+3 bytes | A length-prefixed (uint32) UTF-8 string                      |
| longbinary | 4 to 2^32+3 bytes | Length-prefixed (uint32) binary data                         |

## Packet Format

Packets start with the packet ID (a uint16), followed by the length of the packet payload (a uint64), and then the actual data itself.

Note: using a uint64 to store the payload size is overkill, but it prevents an edge case where the length of a payload (such as a max size longbinary) including the size of the longbinary, is too big to fit into a uint32.

## Terminology

| Term                   | Definition                                                   |
| ---------------------- | ------------------------------------------------------------ |
| Clientbound            | A packet from the server being sent to the client            |
| Serverbound            | A packet from the client being sent to the server            |
| Bidirectional          | A packet that can either originate from the client or the server |
| Pre-authenticate state | The state before the client has sent an Authenticate packet. |
| Dungeon State          | The state in which dungeon-related information is currently being sent, such as attacks, monsters, and dungeon events. |
| Overworld State        | The state in which overworld-related information is currently being sent, such as NPC dialogue and cutscenes. The client should always default to the overworld state upon connecting to the server |

## Chat Formatting

Chat messages can be colored using chat formatting codes. Formatting codes start with the & sign. An & sign can be escaped by doubling it (ex: &&). Simple colors can be encoded by using the string &cX, where X is a character in this table:

| Character | Color Name |
| --------- | ---------- |
| 0         | Red        |
| 1         | Yellow     |
| 2         | Green      |
| 3         | Cyan       |
| 4         | Blue       |
| 5         | White      |
| 6         | Black      |

The sequence &hXXXXXX specifies a 24-bit color encoded using hex (such as &c00FF00).

The string &n can be used to delimit a line break, in case the ASCII character 0x0A can not be used.

## Packets

### 0x0000 - Keep Alive (Bidirectional)

The server will send a keepalive regularly to make sure that the client is still responding. The client must send the same packet with the same Keep Alive ID. After 20 seconds with no reply, the server will kick the client with the reason "Timed Out". The client will also send keepalive packets to the server, and will disconnect if a response from the server hasn't been received for 20 seconds.

### 0x0001 Connection Start (Serverbound)

Sent when the client wants to start a connection with the server and check protocol compatibility. This packet should only be sent once in the lifetime of a connection.

| Type   | Description                                                  |
| ------ | ------------------------------------------------------------ |
| uint16 | The protocol version of the client. It is up to the server to decide if it is compatible with the client. |

If the server is incompatible with the client, a kick packet will be sent with a message along the lines of "You are using an outdated version of OpenPMD! Please use {version}" if the client is outdated, or "The server is using an outdated version of OpenPMD! Please use {version}" if the server is outdated.

If the protocol versions are compatible, the Connection Accept packet will be sent from the server, and the server will wait for the client to send an Authenticate packet.

### 0x0001 Connection Accept (Clientbound)

Sent in response to a Connection Start packet, indicating that the server is compatible with the client, and that the client should begin to authenticate. 

### 0x0002 Authenticate Basic (Serverbound)

Contains information used to authenticate the client. As of now, this is only a username, as proper authentication servers are yet to be implemented. When they are implemented, a new packet (Authenticate Secure) will be added.

| Type   | Description                                                  |
| ------ | ------------------------------------------------------------ |
| string | The username of the player attempting to log in. This must be at most 24 characters long, and consist of only alphanumeric characters, and the characters _ and -. Players providing invalid usernames that do not conform to these requirements should be kicked for the reason "Invalid Username" |

### 0x0003 Authenticate Accept (Clientbound)

Sent in response to an Authenticate packet, denoting that the server has accepted the client's authentication request. This packet should be followed with packets such as Set Control Target

**Payload**

| Type   | Description                                                  |
| ------ | ------------------------------------------------------------ |
| uint32 | The connection ID that the server has assigned to the player |

### 0x0004 Enter Dungeon (Clientbound)

Must be sent in the Overworld state. Signifies that the server is moving the client into a dungeon.

**Payload**

| Type   | Description                                                  |
| ------ | ------------------------------------------------------------ |
| string | The name of the dungeon that the client is entering. Will be displayed on the dungeon transition screen |
| uint8  | The floor of the dungeon that the client is entering.        |
| string | The name of the tileset that the client should use to render this dungeon. |
| uint32 | The ID of the dungeon that the client is entering. This is the number that the server uses to identify this specific dungeon out of the multiple dungeons currently active on the server. |

### 0xFFFF Kick (Clientbound)

Sent before the server closes the connection to the client, containing the reason for the kick. This may not always be the case, however. For example, in the event of a server crash, the connection might be closed without the server sending a kick packet. 

**Payload**

| Type   | Description                                                  |
| ------ | ------------------------------------------------------------ |
| string | The reason why the client was kicked. May contain chat formatting codes. |

## Protocol Versions

| Protocol Version Number | Corresponding Game Version        |
| ----------------------- | --------------------------------- |
| 1                       | OpenPMD Alpha 0.0.1 **(current)** |

