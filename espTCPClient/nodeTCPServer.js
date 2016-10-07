var express= require('express');
var app = express();
var http = require('http').Server(app);
var net = require('net');
var localIP = "";

var server = net.createServer(function(socket){

}).listen(3000, localIP);

server.on('connection', function(socket){
	console.log("Biri baglandi");

	socket.on('data', function(data){
		console.log("%s", data);
	});

	setInterval(function(){

		var x1 = Math.floor((Math.random() * 254) + 1);
		var x2 = Math.floor((Math.random() * 254) + 1);
		var x3 = Math.floor((Math.random() * 254) + 1);

		var getString = "rgb>" + "red=" + x1.toString() + "green=" + x2.toString() + "blue=" + x3.toString() + "#";

		socket.write(getString);
	}, 1000);

});

//http ile TCP soket aynı portta olmamalılar!
http.listen(2500, function(){
	console.log("Listening Port: 2500");
});
