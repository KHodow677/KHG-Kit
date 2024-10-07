var net = require("net");
var clients = [];
var clientData = {};
var rooms = {};

function get_other_client_in_room(roomCode, socket) {
  const room = rooms[roomCode];
  return room ? room.clients.find(c => c.sock !== socket) : undefined;
}

var server = net.createServer(function(socket) {
  const client = { sock: socket, room: null };
  clients.push(client);
  console.log("A client connected");
  socket.on("data", function(data) {
    const request = data.toString();
    if (request.startsWith("POST /room/create")) {
      const body = request.split("\r\n\r\n")[1];
      try {
        const parsed_data = JSON.parse(body);
        const roomCode = parsed_data.roomCode;
        if (rooms[roomCode]) {
          socket.write("HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nRoom already exists");
        } 
        else {
          rooms[roomCode] = { clients: [client] };
          client.room = roomCode;
          socket.write(`HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nRoom ${roomCode} created`);
          console.log(`Room ${roomCode} created by a client`);
        }
      } 
      catch (e) {
        console.error("Error parsing JSON:", e);
        socket.write("HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nInvalid JSON");
      }
    } 
    else if (request.startsWith("POST /room/join")) {
      const body = request.split("\r\n\r\n")[1];
      try {
        const parsed_data = JSON.parse(body);
        const roomCode = parsed_data.roomCode;
        if (rooms[roomCode]) {
          const room = rooms[roomCode];
          if (room.clients.length >= 2) {
            socket.write("HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nRoom is full");
          } 
          else {
            room.clients.push(client);
            client.room = roomCode;
            socket.write(`HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nJoined room ${roomCode}`);
            console.log(`A client joined Room ${roomCode}`);
          }
        } 
        else {
          socket.write("HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nRoom not found");
        }
      } 
      catch (e) {
        console.error("Error parsing JSON:", e);
        socket.write("HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nInvalid JSON");
      }
    } 
    else if (request.startsWith("POST /send")) {
      const body = request.split("\r\n\r\n")[1];
      try {
        const parsed_data = JSON.parse(body);
        if (client.room) {
          const other_client = get_other_client_in_room(client.room, socket);
          if (other_client) {
            other_client.sock.write(`HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n${parsed_data.message}`);
          } 
          else {
            socket.write("HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nNo other client in the room");
          }
        } 
        else {
          socket.write("HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nClient not in any room");
        }
      } 
      catch (e) {
        console.error("Error parsing JSON:", e);
        socket.write("HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nInvalid JSON");
      }

    } 
    else if (request.startsWith("GET /receive")) {
      console.log("Client requested data.");
      const response = clientData.message ? clientData.message : 'No message available';
      socket.write(`HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n${response}`);
    } 
    else {
      socket.write("HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nServer not found");
    }
  });
  socket.on("end", function() {
    console.log("A client disconnected");
    clients = clients.filter(c => c.sock !== socket);
  });
  socket.on("error", function(err) {
    console.error("Error: ", err);
    clients = clients.filter(c => c.sock !== socket);
  });
});

server.listen(3000, function() {
  console.log("HTTP server started on port 3000");
});

