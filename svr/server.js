const express = require('express');
const http = require('http');
const WebSocket = require('ws');

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

const clients = new Map();
let client_id_counter = 16777215;

wss.on('connection', (ws) => {
  const client_id = client_id_counter.toString(16).padStart(6, "0").toUpperCase();
  client_id_counter--;
  clients.set(ws, { id: client_id });

  console.log(`Client ${client_id} connected`);

  ws.on('message', (message) => {
    try {
      const data = JSON.parse(message);
      if (data.type === 'send') {
        const { receiverId, message } = data.payload;
        for (let [clientWs, client] of clients) {
          if (client.id === receiverId) {
            clientWs.send(JSON.stringify({ type: 'message', message }));
          }
        }
      } else if (data.type === 'receive') {
        ws.send(JSON.stringify({ type: 'message', message: 'No new messages' }));
      }
    } catch (e) {
      console.error('Error handling message:', e);
      ws.send(JSON.stringify({ type: 'error', message: 'Invalid JSON format' }));
    }
  });

  ws.on('close', () => {
    console.log(`Client ${clients.get(ws).id} disconnected`);
    clients.delete(ws);
    client_id_counter++;
  });

  ws.on('error', (error) => {
    console.error(`Error with client ${clients.get(ws).id}:`, error);
  });
});

server.listen(3000, () => {
  console.log('Server is listening on port 3000');
});
