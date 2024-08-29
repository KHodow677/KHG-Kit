var net = require('net');
var clients = [];
var server = net.createServer(function(socket) {
  console.log('Client connected');
  clients.push(socket);
  socket.on('data', function(data) {
    console.log('Received:', data.toString());
    clients.forEach(function(client) {
      if (client !== socket) {
        client.write(data);
      }
    });
  });
  socket.on('end', function() {
    console.log('Client disconnected');
    clients = clients.filter(client => client !== socket);
  });
  socket.on('error', function(err) {
    console.error('Socket error:', err);
    clients = clients.filter(client => client !== socket);
  });
});
server.listen(8080, function() {
  console.log('TCP server started on port 8080');
});
