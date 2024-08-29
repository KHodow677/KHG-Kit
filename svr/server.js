var connect = require('connect');
var http = require('http');
var bodyParser = require('body-parser');
var serveStatic = require('serve-static');
var app = connect();
app.use(serveStatic('app', {
    'index': ['index.html', 'index.htm', 'default.html']
}));
app.use(bodyParser.urlencoded({ extended: true }));
app.use(function(req, res) {
    console.log('Request received.');
    res.writeHead(200, { 'Content-Type': 'text/plain' });
    res.end('Request processed.');
});
http.createServer(app).listen(80);
console.log('Server started at http://localhost:80');
