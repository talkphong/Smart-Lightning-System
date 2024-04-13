var fs = require('fs');
var url = require('url');
var http = require('http');
var WebSocket = require('ws');
//////////////////// SQL ////////////////////////
const mysql = require("mysql");
const db = mysql.createConnection({
    user: "root",
    host: "localhost",
    password: "",
    database: "iot_autoled",
});

let tuDong = -1, den1State = -1, den2State = -1;
/////////////////////////////////////////////////
// function gửi yêu cầu(response) từ phía server hoặc nhận yêu cầu (request) của client gửi lên
function requestHandler(request, response) {
    fs.readFile('./index.html', function (error, content) {
        response.writeHead(200, {
            'Content-Type': 'text/html'
        });
        response.end(content);
    });
}
// create http server
var server = http.createServer(requestHandler);
var ws = new WebSocket.Server({
    server
});
var clients = [];

function broadcast(socket, data) {
    console.log(clients.length);
    for (var i = 0; i < clients.length; i++) {
        if (clients[i] != socket) {
            clients[i].send(data);
        }
    }
}
ws.on('connection', function (socket, req) {
    clients.push(socket);
    socket.on('message', function (message) {
        console.log('received: %s', message);
        let now = new Date;
        var date = now.getFullYear() + '-' + (now.getMonth() + 1) + '-' + now.getDate();
        var time = now.getHours() + ":" + now.getMinutes() + ":" + now.getSeconds();

        var json = JSON.parse(message);


        if (json["tuDong"] != tuDong || json["den1State"] != den1State || json["den2State"] != den2State) {
            db.query("INSERT INTO `event` (`eventID`, `Date`, `Time`, `State`, `LUX`, `PIR`, `Led1`, `RtcStart`, `RtcEnd`, `Led2`) " +
                "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);",
                ['NULL', date, time, json["tuDong"], json["lux"], json["chuyenDong"], json["den1State"], json["rtcStart"], json["rtcEnd"], json["den2State"]]);
            tuDong = json["tuDong"];
            den1State = json["den1State"];
            den2State = json["den2State"];
        }

        broadcast(socket, message);
    });
    socket.on('close', function () {
        var index = clients.indexOf(socket);
        clients.splice(index, 1);
        console.log('disconnected');
    });
});
server.listen(3000);
console.log('Server listening on port 3000');
