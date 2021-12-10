// DotENV setup
require('dotenv').config();
const Particle = require('particle-api-js');
const redis = require('redis');
const NRP = require('node-redis-pubsub');
const { promisify } = require('util');
const { MongoClient } = require('mongodb');

const connectionString = process.env.MONGODB_URI;
const databaseName = 'test';

const mongo = new MongoClient(`mongodb://${connectionString}?retryWrites=true`, {
    useUnifiedTopology: true,
});
mongo.connect();
const database = mongo.db(databaseName);

const redisConfig = {
  port: 6379,
  scope: 'romcon',
};

console.log("REDIS_HOST is set to: ");
console.log(process.env.REDIS_HOST);

const redisClient = redis.createClient({
  host: process.env.REDIS_HOST,
  port: 6379,
});
redisClient.on('error', (err) => {
  console.log(err);
});
redisClient.setAsync = promisify(redisClient.set).bind(redisClient);
redisClient.getAsync = promisify(redisClient.get).bind(redisClient);
redisClient.expireAsync = promisify(redisClient.expire).bind(redisClient);

const nrp = new NRP(redisConfig);
nrp.on('error', (err) => {
  console.log(err);
});

const particle = new Particle();

const particleAPISession = {
  username: process.env.PARTICLE_USERNAME,
  password: process.env.PARTICLE_PASSWORD,
  token: undefined,
  expires_in: undefined,
  refresh_token: undefined,
  event_stream: {},
  deviceList: {},
  keepAliveList: {},
  connected: false,
};

async function login() {
  if (particleAPISession.connected && particleAPISession.token) {
    return particleAPISession.token;
  }
  if (particleAPISession.username && particleAPISession.password) {
    try {
      const loginSession = await particle.login({ username: particleAPISession.username, password: particleAPISession.password});
      particleAPISession.token = loginSession.body.access_token;
      particleAPISession.connected = true;
      return particleAPISession.token;
    } catch(err) {
      particleAPISession.connected = false;
      return Promise.reject(Error('Unable to login.'));
    }
  } else {
    return Promise.reject(Error('Credentials required.'))
  }
}

(async function run() {
  const token = await login();
  const getDevices = await particle.listDevices({ auth: token });
  Object.keys(getDevices.body).forEach(async (key) => {
    const device = getDevices.body[key];
    console.log(`Found device: ${device.name}`);
    if(device.online) {
      nrp.emit('online', device.id);
    } else {
      nrp.emit('offline', device.id);
    }
    await redisClient.setAsync(`${device.id}_status`, device.online);
      await redisClient.expireAsync(`${device.id}_status`, 30);
    const statusStream = await particle.getEventStream({ deviceId: device.id, name: "spark/status", auth: token });
    statusStream.on('event', async function (event) {
      const status = event.data === 'online';
      console.log(`Device {${device.name}}(${event.coreid}) registered as '${event.data}'`);
      if(status) {
        nrp.emit('online', device.id);
      } else {
        nrp.emit('offline', device.id);
      }
      await redisClient.setAsync(`${device.id}_status`, status);
      await redisClient.expireAsync(`${device.id}_status`, 30);
    });
    const logStream = await particle.getEventStream({ deviceId: device.id, name: "romcon", auth: token });
    logStream.on('event', async function (data) {
      const incomingEvent = data;
      incomingEvent.data = JSON.parse(incomingEvent.data);
      if(incomingEvent.data.datetime != 'undefined') {
          incomingEvent.data.datetime = new Date(incomingEvent.data.datetime * 1000); // Particle sends time as seconds since 1970, JS expects milliseconds.
      } else {
          incomingEvent.data.datetime = new Date(Date(incomingEvent.published_at));
      }
      const collection = database.collection(device.id);
      await collection.insertOne(incomingEvent.data);
      nrp.emit('online', device.id);
      await redisClient.setAsync(`${device.id}_status`, true);
      await redisClient.expireAsync(`${device.id}_status`, 30);
    });
  });
})();
