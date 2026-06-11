/*
Brandon Namgoong
TEJ4M1
FPT
2026-06-##
*/

// NOTE: This server.js has mostly been copied over from the server.js from my Node.js Final Project.
// (i mean... it's boilerplate)

// packages
const express = require("express");
const http = require("http");
const { SerialPort } = require("serialport");
const { ReadlineParser } = require("@serialport/parser-readline");
const { Server } = require("socket.io");

const app = express();
const server = http.createServer(app);
const io = new Server(server);

// constants
const PORT = 3000;
const ARDUINO_SERIAL_PORT = "PLACEHOLDER";

// arduino setup
let arduino = new SerialPort({
    path: ARDUINO_SERIAL_PORT,
    baudRate: 9600,
    dataBits: 8,
    parity: "none",
    stopBits: 1,
    flowControl: false
});
arduino.pipe(new ReadlineParser({ delimiter: "\r\n" }).on("data", (data) => { // pipe serial stream to delimiter parser
    io.emit("msg", { value: data.trim() }); // emit to io
}));
arduino.on("open", () => {
    console.log("Serial port " + ARDUINO_SERIAL_PORT + " is opened.");
});

// express setup
app.use(express.static("public"));
app.get("/", (req, res) => { // load website
    res.sendFile("index.html");
});
app.get("/:action", (req, res) => {
    let action = req.params.action || req.param("action"); // no clue wth this is

    arduino.write(action); // send to arduino
    res.send("Message " + action + " sent");
});

// express + socket.io server
server.listen(PORT, () => {
    console.log("Arduino server listening at http://localhost:" + PORT);
});