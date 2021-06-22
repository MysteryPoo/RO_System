require('dotenv').config();
const Particle = require('particle-api-js');

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
    particleAPISession.connected = false;
    return Promise.reject(Error('Credentials required.'));
  }
}

async function sendConfiguration(deviceId, newConfig = null) {
  const token = await login();
  const configurationStored = await api.getConfiguration(deviceId);
  const configuration = configurationStored || {};
  if (newConfig) {
    for(key in newConfig) {
      configuration[key] = newConfig[key];
    }
  }
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

async function getDeviceList() {
  const token = await login();
  const getDevices = await particle.listDevices({ auth: token });
  const deviceList = [];
  Object.keys(getDevices.body).forEach(async (key) => {
    const device = getDevices.body[key];
    deviceList.push({
      id: device.id,
      name: device.name,
    });
  });
  return deviceList;
}

module.exports = {
  sendConfiguration,
  getDeviceList,
};
