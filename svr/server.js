var connect = require('connect');
var http = require('http');
var bodyParser = require('body-parser');
var serveStatic = require('serve-static');
var app = connect();
let clientData = {};
app.use(serveStatic('app', {
    'index': ['index.html', 'index.htm', 'default.html']
}));
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());
app.use(function(req, res) {
  if (req.method === 'POST' && req.url === '/send') {
    console.log('Data received from client:', req.body);
    clientData = req.body;
    res.end('Data stored on the server');
  } 
  else if (req.method === 'GET' && req.url === '/receive') {
    console.log('Client requested data.');
    res.end(clientData.message || 'No message available');
  } 
  else {
    res.end('Server not found');
  }
});

http.createServer(app).listen(80);
console.log('Server started at http://localhost:80');
