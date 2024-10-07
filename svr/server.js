var net = require("net");
var rooms = {};

var server = net.createServer(function(socket) {
  console.log("Client " + client_id + " connected");
  socket.on("data", function(data) {
    const request = data.toString();
    if (request.startsWith("POST /send")) {
      const body = request.split("\r\n\r\n")[1];
      try {
        const parsed_data = JSON.parse(body);
        const command = parsed_data.command;
        
        if (command === "create_room") {
          const room_code = parsed_data.room_code;
          console.log(`Room ${room_code} created by client ${client_id}`);
          socket.write(`HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nRoom ${room_code} created.`);
          rooms[room_code] = {client1: socket, client2: null};
        } 
        else if (command === "join_room") {
          const room_code = parsed_data.room_code;
          console.log(`Client ${client_id} joined room ${room_code}`);
          socket.write(`HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nJoined room ${room_code}.`);
          rooms[room_code].client2 = socket;
        } 
        else if (command === "send_message") {
          const room_code = parsed_data.room_code;
          const message = parsed_data.message;
          console.log(`Client sent_message "${message}"`);
          if (rooms[room_code].client1 === socket || rooms[room_code].client2 === socket) {
            rooms[room_code].client1.write(`HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nSend message "${message}"`);
            rooms[room_code].client2.write(`HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nSend message "${message}"`);
          }
          else {
            rooms[room_code].client1.write(`HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nWrong Room`);
            rooms[room_code].client2.write(`HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nWrong Room`);
          }
        } 
        else {
          socket.write("HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nInvalid Command");
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
    console.log("Client " + get_client_id(socket) + " disconnected");
    clients = clients.filter(client => client.sock !== socket);
    client_id_counter++;
  });

  socket.on("error", function(err) {
    console.error("KHG Error: ", err);
    clients = clients.filter(client => client.sock !== socket);
  });
});

server.listen(3000, function() {
  console.log("HTTP server started on port 3000");
});
