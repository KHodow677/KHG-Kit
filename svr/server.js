//var http = require("http");
//var net = require("net");
//let client_id_counter = 16777215;
//var clients = [];
//
//function get_client_id(socket) {
//  const client = clients.find(client => client.sock === socket);
//  return client ? client.id : undefined;
//}
//
//var server = net.createServer(function(socket) {
//  const client_id = client_id_counter.toString(16).padStart(6, "0").toUpperCase();
//  client_id_counter--;
//  const client = { id: client_id, sock: socket };
//  clients.push(client);
//  console.log("Client " + client_id + " connected");
//  socket.on("data", function(data) {
//    const data_split = data.toString().split(":");
//    console.log("Received:", data_split[0]);
//    clients.forEach(function(client) {
//      if (client.id == data_split[0]) {
//        client.sock.write(data_split[1]);
//      }
//    });
//  });
//  socket.on("end", function() {
//    console.log("Client " + get_client_id(socket) + " disconnected");
//    clients = clients.filter(client => client.sock !== socket);
//    client_id_counter++;
//  });
//  socket.on("error", function(err) {
//    console.error("KHG Error: ", err);
//    clients = clients.filter(client => client.sock !== socket);
//  });
//});
//
////server.listen(80, function() {
////  console.log("HTTP server started on port 80");
////});
//
//
//var connect = require('connect');
//var http = require('http');
//var bodyParser = require('body-parser');
//var serveStatic = require('serve-static');
//var app = connect();
//let clientData = {};
//app.use(serveStatic('app', {
//    'index': ['index.html', 'index.htm', 'default.html']
//}));
//app.use(bodyParser.urlencoded({ extended: true }));
//app.use(bodyParser.json());
//app.use(function(req, res) {
//  if (req.method === 'POST' && req.url === '/send') {
//    console.log('Data received from client:', req.body);
//    clientData = req.body;
//    res.end('Data stored on the server');
//  } 
//  else if (req.method === 'GET' && req.url === '/receive') {
//    console.log('Client requested data.');
//    res.end(clientData.message || 'No message available');
//  } 
//  else {
//    res.end('Server not found');
//  }
//});
//http.createServer(app).listen(80);
//console.log('Server started at http://localhost:80');

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
        const parsed_receiver = parsed_data.message.toString().split(":")[0];
        parsed_data.message = parsed_data.message.toString().split(":")[1];
        clients.forEach(function(client) {
          if (client.id == parsed_receiver) {
            client.sock.write(`HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n${parsed_data.message}`);
          }
        });
        clientData = parsed_data;
        socket.write(`HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n${parsed_data.message}`);
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

server.listen(80, function() {
  console.log("HTTP server started on port 80");
});

