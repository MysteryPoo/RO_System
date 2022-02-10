const udp = require('dgram');

const server = udp.createSocket('udp4');

server.on('error', (err) => {
  console.error(err);
  server.close();
});

server.on('message', (msg, info) => {
  console.log('Data received from client : ' + msg.toString());
  console.log('Received %d bytes from %s:%d\n',msg.length, info.address, info.port);

  if (msg.toString() === 'Looking for MQTT server') {
    console.log('Sending connection information to client.');
    console.log('Client info:');
    console.log(`Address: ${info.address}`);
    console.log(`Port: ${info.port}`);
    console.log(`Override: ${process.env.MQTT_OVERRIDE ?? '<Blank>'}`);
    server.send(JSON.stringify({
      username: process.env.MQTT_USERNAME,
      password: process.env.MQTT_PASSWORD,
      override: process.env.MQTT_OVERRIDE ?? ''
    }), info.port, info.address);
  }
});

server.on('listening',function(){
  var address = server.address();
  var port = address.port;
  var family = address.family;
  var ipaddr = address.address;
  console.log('Server is listening at port : ' + port);
  console.log('Server ip : ' + ipaddr);
  console.log('Server is IP4/IP6 : ' + family);
});

//emits after the socket is closed using socket.close();
server.on('close',function(){
  console.log('Socket is closed !');
});

server.bind(1882);
