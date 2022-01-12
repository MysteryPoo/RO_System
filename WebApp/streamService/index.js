// DotENV setup
require('dotenv').config();
const Particle = require('particle-api-js');
const { MongoClient } = require('mongodb');

const connectionString = process.env.MONGODB_URI ?? 'localhost';
const databaseName = process.env.MONGODB_DB_NAME ?? 'test';

const mongo = new MongoClient(`mongodb://${connectionString}?retryWrites=true`, {
    useUnifiedTopology: true,
});
mongo.connect();
const database = mongo.db(databaseName);

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

async function getConfiguration(device) {
  const query = {
      deviceId: device,
  };
  const configuration = database.collection('configuration').findOne(query);
  return configuration;
}

async function sendConfiguration(deviceId) {
  const token = await login();
  const configurationStored = await getConfiguration(deviceId);
  const configuration = configurationStored || {};
  if (configuration != {}) {
    try {
      const response = await particle.callFunction({ deviceId: deviceId, name: 'configuration', argument: JSON.stringify(configuration), auth: token });
      console.log(response);
      return response;
    } catch(err) {
      console.log(err);
      particleAPISession.connected = false;
      // Try again
      return sendConfiguration(deviceId, configuration);
    }
  }
}

async function UpdateDeviceStatus(device) {
  const collection = database.collection('status');
  const query = {
    'deviceId': device.id
  };
  const update = {
    $set: {
      'deviceId': device.id,
      'online': device.online,
      'last_heard': new Date(device.last_heard)
    }
  };
  const options = {
    upsert: true
  };
  await collection.updateOne(query, update, options);

  if (device.online) {
    sendConfiguration(device.id);
  }
}

(async function run() {
  // Login
  const token = await login();
  // Get Device List
  const getDevices = await particle.listDevices({ auth: token });
  // Setup each device
  Object.keys(getDevices.body).forEach(async (key) => {
    const device = getDevices.body[key];
    console.log(`Found device: ${device.name}`);
    UpdateDeviceStatus(device);
    // Setup Status stream
    const statusStream = await particle.getEventStream({ deviceId: device.id, name: "spark/status", auth: token });
    statusStream.on('event', async function (event) {
      const status = event.data === 'online';
      console.log(`Device {${device.name}}(${event.coreid}) registered as '${event.data}'`);
      UpdateDeviceStatus({id: device.id, online: status});
    });
    // Setup Logger stream
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
    });
  });
})();
