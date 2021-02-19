// DotENV setup
require('dotenv').config();

// Core includes
const express = require('express');
const cors = require('cors');
const bodyParser = require('body-parser');
const morgan = require('morgan');

// API includes
const Particle = require('particle-api-js');
const api = require('./db/api');

// Global objects
const particle = new Particle();
const app = express();

const particleAPISession = {
    username: process.env.PARTICLE_USERNAME,
    password: process.env.PARTICLE_PASSWORD,
    token: undefined,
    expires_in: undefined,
    refresh_token: undefined,
    valid: false,
    event_stream: {},
    deviceList: {},
    keepAliveList: {},
};
 
app.use(morgan('tiny'));
app.use(cors());
app.use(bodyParser.json());
app.use(checkAuth);

function checkAuth(req, res, next) {
    if(req.query.secret === process.env.API_SECRET) {
        return next();
    } else {
        return res.json({
            error: 'Unauthorized'
        });
    }
}
 
app.get('/', (req, res) => {
    res.json({
        message: 'Behold The MEVN Stack!'
    });
});

app.get('/deviceList', (req, res) => {
    res.send(particleAPISession.deviceList);
});

app.get('/:deviceId/lastTick', (req, res) => {
    api.getLastTick(req.params.deviceId).then( (data) => {
        res.json(data);
    });
});

app.get('/:deviceId/currentState', (req, res) => {
    api.getCurrentState(req.params.deviceId).then( (data) => {
        res.json(data);
    });
});

app.get('/:deviceId/pumpState', (req, res) => {
    api.getPumpStates(req.params.deviceId).then( (data) => {
        res.json(data);
    });
});

app.get('/:deviceId/status', (req, res) => {
    res.send({
        online: particleAPISession.deviceList[req.params.deviceId].online
    });
});

app.get('/:deviceId/configuration', (req, res) => {
    api.getConfiguration(req.params.deviceId).then( (data) => {
        res.json(data);
    });
});

app.post('/:deviceId/configuration', (req, res) => {
    api.setConfiguration(req.params.deviceId, req.body);
    sendEnableConfig(req.params.deviceId, req.body);
    sendFillLevelConfig(req.params.deviceId, req.body);
    res.send(req.body);
});
 
const port = process.env.PORT || 4000;
app.listen(port, () => {
    console.log(`listening on ${port}`);
});

particle.login({ username: particleAPISession.username, password: particleAPISession.password}).then(
    function(data) {
        particleAPISession.password = undefined;
        particleAPISession.token = data.body.access_token;
        particleAPISession.valid = true;

        particle.listDevices({ auth: particleAPISession.token }).then(function (devices) {
            for(device of devices.body) {
                console.log(`Found device: ${device.name}`);
                particleAPISession.deviceList[device.id] = device;
                api.addDevice(device.id);
            }
            devices.body.forEach( (device) => {
                particle.getEventStream({ deviceId: device.id, name: "spark/status", auth: particleAPISession.token }).then(function (stream) {
                    console.log(`Setting up Heart Monitor for: ${device.name}`);
                    stream.on('event', function (data) {
                        console.log(JSON.stringify(data));
                        console.log(`Device {${particleAPISession.deviceList[data.coreid].name}} registered as {${data.data}}`);
                        particleAPISession.deviceList[data.coreid].online = data.data === "online" ? true : false;
                        if (particleAPISession.deviceList[data.coreid].online) {
                            api.getConfiguration(data.coreid).then( (config) => {
                                if (config.length > 0) {
                                    console.log(JSON.stringify(config));
                                    const configuration = config[0];
                                    sendEnableConfig(data.coreid, configuration);
                                    sendFillLevelConfig(data.coreid, configuration);
                                }
                            });
                        }
                    });
                });
                particle.getEventStream({ deviceId: device.id, name: "romcon", auth: particleAPISession.token }).then(function (stream) {
                    console.log(`Stream setup for ${device.name}`);
                    particleAPISession.event_stream[device.id] = stream;
                    stream.on('event', function (data) {
                        const dataParsed = data;
                        dataParsed.data = JSON.parse(dataParsed.data);
                        if(dataParsed.data.datetime != 'undefined') {
                            dataParsed.data.datetime = new Date(dataParsed.data.datetime * 1000); // Particle sends time as seconds since 1970, JS expects milliseconds.
                        } else {
                            dataParsed.data.datetime = new Date(Date(dataParsed.published_at));
                        }
                        console.log(JSON.stringify(dataParsed));
                        api.storeMessage(dataParsed.coreid, dataParsed);
                        statusCheck(device, particleAPISession.deviceList[device.coreid].online, true);
                        particleAPISession.deviceList[device.coreid].online = true;
                    });
                });
            });
        },
        function (err) {
            console.log(err);
        });

    },
    function(err) {
        console.log(err);
    }
);

const sendEnableConfig = function (deviceId, config) {
    if (config.enabled) {
        if (particleAPISession.deviceList[deviceId].online) {
            particle.callFunction({ deviceId: deviceId, name: 'setEnable', argument: config.enabled ? 'true' : 'false', auth: particleAPISession.token }).then(
                function (data) {
                    console.log(JSON.stringify(data));
                },
                function (error) {
                    console.log(error);
                    sendEnableConfig(deviceId, config);
                }
            );
        }
    }
};
const sendFillLevelConfig = function (deviceId, config) {
    if (config.fillStart && config.fillStop) {
        if (particleAPISession.deviceList[deviceId].online) {
            particle.callFunction({ deviceId: deviceId, name: 'setFillDistances', argument: `${config.fillStart},${config.fillStop}`, auth: particleAPISession.token }).then(
                function (data) {
                    console.log(JSON.stringify(data));
                },
                function (error) {
                    console.log(error);
                    sendFillLevelConfig(deviceId, config);
                }
            );
        }
    }
};