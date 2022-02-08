// DotENV setup
require('dotenv').config();
const Particle = require('particle-api-js');
const { MongoClient } = require('mongodb');
const mqtt = require('mqtt');

const databaseName = process.env.MONGODB_DB_NAME ?? 'test';
const username = process.env.MONGODB_USERNAME ?? 'admin';
const password = process.env.MONGODB_PASSWORD ?? 'admin';
const connectionString = process.env.MONGODB_URI ?? 'localhost';

const mongo = new MongoClient(`mongodb://${username}:${password}@${connectionString}?retryWrites=true&authSource=admin`, {
  useUnifiedTopology: true,
});
mongo.connect();
const database = mongo.db(databaseName);

const particle = new Particle();

const deviceTimeouts = {};

const particleAPISession = {
  username: process.env.PARTICLE_USERNAME,
  password: process.env.PARTICLE_PASSWORD,
  token: undefined,
  expires_in: undefined,
  refresh_token: undefined,
  event_stream: {},
  deviceList: [],
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
    //sendConfiguration(device.id);
  }
}

function ResetTimer(deviceId) {
  if (deviceTimeouts[deviceId]) {
    clearTimeout(deviceTimeouts[deviceId]);
  }
  deviceTimeouts[deviceId] = setTimeout( () => {
    UpdateDeviceStatus({id: deviceId, online: false});
    deviceTimeouts[deviceId] = undefined;
    delete deviceTimeouts[deviceId];
  }, 30000);
}

(async function runParticle() {
  // Login
  const token = await login();
  // Get Device List
  const getDevices = await particle.listDevices({ auth: token });
  // Setup each device
  Object.keys(getDevices.body).forEach(async (key) => {
    const device = getDevices.body[key];
    UpdateDeviceStatus(device);
    // Setup Status stream
    const statusStream = await particle.getEventStream({ deviceId: device.id, name: "spark/status", auth: token });
    statusStream.on('event', async function (event) {
      const status = event.data === 'online';
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

(async function runmqtt() {
  const options = {
    clientId: "streamService",
    username: process.env.MQTT_USERNAME,
    password: process.env.MQTT_PASSWORD,
    clean: true,
  };
  const client = mqtt.connect("mqtt://rabbitmq", options);
  client.on('connect', () => {
    console.log('MQTT: Connected!');
  });
  client.subscribe("from/#", {qos: 1});
  client.on('message', async (topic, message, packet) => {
    const tokens = topic.split('/');
    const deviceId = tokens[1]; // TODO: Validate this value against a Particle.getDevices call
    const subTopic = tokens[2];
    if (subTopic === 'status') {
      ResetTimer(deviceId);
      UpdateDeviceStatus({id: deviceId, online: true});
    } else if (subTopic === 'romcon') {
      try {
        const data = JSON.parse(message);
        if (data.datetime != 'undefined') {
          data.datetime = new Date(data.datetime * 1000); // Particle sends time as seconds since 1970, JS expects milliseconds.
        } else {
          data.datetime = new Date();
        }
        const collection = database.collection(deviceId);
        await collection.insertOne(data);

        ResetTimer(deviceId);
      } catch (e) {
        console.error(`Failed to parse the following as JSON: ${message}`);
      }
    } else if (subTopic === 'configuration') {
      const component = tokens[3];
      const feature = JSON.parse(message);
      feature['component'] = component;
      feature['deviceId'] = deviceId;

      const query = {
        deviceId,
        component
      };
      const update = {
        $set: feature
      };
      const collection = database.collection('feature');
      const options = {
        upsert: true
      };
      await collection.updateOne(query, update, options);
    }
  });
})();
