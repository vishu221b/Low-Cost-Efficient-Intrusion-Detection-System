const path = require('path');
const express = require('express');
const WebSocket = require('ws');
const mqtt = require("mqtt")
const appConstants = require("./utils/AppConstants")

const app = express();

const client = mqtt.connect(`mqtt://${appConstants.MQTT_SERVER}`)

app.use('/static', express.static(path.join(__dirname, 'public')));

let clients = [];

const HTTP_PORT = 8080;
let devices = {
	esp32: { port: 8888 },
};

process.on('uncaughtException', (error, origin) => {
	console.log('----- Uncaught exception -----');
	console.log(error);
	console.log('----- Exception origin -----');
	console.log(origin);
	console.log('----- Status -----');
});


client.on("connect", () => {
	console.log("Connected MQTT!!")
	client.subscribe(appConstants.MQTT_STREAM_RECEIVER_TOPIC)
})

let _img;


// Clients
const wss = new WebSocket.Server({port: '8999'}, () => console.log(`WS Server is listening at 8999`));

wss.on('connection', ws => {
	ws.on('message', data => {
		if (ws.readyState !== ws.OPEN) return;
		
		console.log(data)
		clients.push(ws);
		ws.send(JSON.stringify({ devices: devices }))
		try {
			data = JSON.parse(data);
			
			if(data.operation === 'command') {
				if(devices[data.command.recipient]) {
					devices[data.command.recipient].command = data.command.message.key + '=' + data.command.message.value;
				}
			}
		} catch (error) {}
	});
});

client.on("message", async (t, msg) => {
	_img=Buffer.from(Uint8Array.from(msg)).toString('base64')
	devices.esp32.image=_img;
	clients[0].send(JSON.stringify({ devices: devices }))
})

// Devices
Object.entries(devices).forEach(([key]) => {
	const device = devices[key];
	
	new WebSocket.Server({port: device.port}, () => console.log(`WS Server is listening at ${device.port}`)).on('connection',(ws) => {
		// ws.on('connection', wa => {
		console.log("Connected to: " + String(device.port))
		// const wa=ws
		ws.on('message', data => {
			if (ws.readyState !== ws.OPEN) 
			{
				console.log("Not open....");
				return;
			}
			console.log(data)
			if (device.command) {
				
				// Consume the command, 
				ws.send(device.command);
				
				// Clear the value from memory
				device.command = null; 
			}

			if (typeof data === 'object') {
				/**
				 * Add image buffer as base64 image
				 */
				// device.image = Buffer.from(Uint8Array.from(data)).toString('base64');
				device.image = _img;
			} else {
				/**
				 * Format LED Data
				 */
				device.peripherals = data.split(",").reduce((acc, item) => {
					const key = item.split("=")[0];
					const value = item.split("=")[1];
					acc[key] = value;
					return acc;
				}, {});
			}

			/**
			 * Send the formed response to the frontend client, connected via 8999
			 * TODO: Instead forward this to MQTT remote server
			 */
			clients.forEach(client => {
				client.send(JSON.stringify({ devices: devices }));
			});
		});
	});
	// });
});

app.get('/client',(_req,res)=>{ res.sendFile(path.resolve(__dirname,'./public/client.html')); });
app.listen(HTTP_PORT,()=>{ console.log(`HTTP server starting on ${HTTP_PORT}`); });