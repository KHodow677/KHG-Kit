var net = require("net");
var clients = [];
var clientData = {};
let client_id_counter = 16777215;

function get_client_id(socket) {
  const client = clients.find(client => client.sock === socket);
  return client ? client.id : undefined;
}

var server = net.createServer(function(socket) {
  const client_id = client_id_counter.toString(16).padStart(6, "0").toUpperCase();
  client_id_counter--;
  const client = { id: client_id, sock: socket };
  clients.push(client);
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
        } 
        else if (command === "join_room") {
          const room_code = parsed_data.room_code;
          console.log(`Client ${client_id} joined room ${room_code}`);
          socket.write(`HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nJoined room ${room_code}.`);
        } 
        else if (command === "send_message") {
          const message = parsed_data.message;
          console.log(`Client ${client_id} sent_message "${message}"`);
          socket.write(`HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nSend message "${message}"`);
        } 
        else {
          const parsed_receiver = parsed_data.message.toString().split(":")[0];
          parsed_data.message = parsed_data.message.toString().split(":")[1];
          clients.forEach(function(client) {
            if (client.id === parsed_receiver) {
              client.sock.write(`HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n${parsed_data.message}`);
            }
          });
          clientData = parsed_data;
          socket.write(`HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n${parsed_data.message}`);
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
