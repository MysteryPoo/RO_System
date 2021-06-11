// DotENV setup
require('dotenv').config();

// Core includes
const express = require('express');
const cors = require('cors');
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
app.use(express.json());
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
    const { deviceId } = req.params;
    const { to, from, resolution } = req.query;
    api.getLastTick(deviceId, new Date(Number(to)), new Date(Number(from)), Number(resolution)).then( (data) => {
        res.json(data);
    })
    .catch(err => {console.log(err)});
});

app.get('/:deviceId/currentState', (req, res) => {
    api.getCurrentState(req.params.deviceId).then( (data) => {
        res.json(data);
    })
    .catch(err => {console.log(err)});;
});

app.get('/:deviceId/pumpState', (req, res) => {
    api.getPumpStates(req.params.deviceId).then( (data) => {
        res.json(data);
    })
    .catch(err => {console.log(err)});;
});

app.get('/:deviceId/status', (req, res) => {
    const device = particleAPISession.deviceList[req.params.deviceId];
    const status = {
        online: device ? device.online : "Unknown"
    };
    res.send(status);
});

app.get('/:deviceId/configuration', (req, res) => {
    api.getConfiguration(req.params.deviceId).then( (data) => {
        res.json(data);
    })
    .catch(err => {console.log(err)});;
});

app.post('/:deviceId/configuration', (req, res) => {
    api.setConfiguration(req.params.deviceId, req.body);
    sendConfiguration(req.params.deviceId, req.body);
    res.send(req.body);
});

app.get('/:deviceId/logs', (req, res) => {
    api.getLog(req.params.deviceId, req.body)
    .then( (data) => {
        res.json(data);
    })
    .catch(err => console.log(err));
});

app.delete('/:deviceId/logs/:logId', (req, res) => {
    api.clearLog(req.params.deviceId, req.params.logId)
    .then((res) => {})
    .catch(err => {console.log(err)});
    res.send();
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
                if(device.online) {
                    sendConfiguration(device.id);
                }
            }
            devices.body.forEach( (device) => {
                particle.getEventStream({ deviceId: device.id, name: "spark/status", auth: particleAPISession.token }).then(function (stream) {
                    console.log(`Setting up Heart Monitor for: ${device.name}`);
                    stream.on('event', function (data) {
                        console.log(JSON.stringify(data));
                        console.log(`Device {${particleAPISession.deviceList[data.coreid].name}} registered as {${data.data}}`);
                        particleAPISession.deviceList[data.coreid].online = data.data === "online";
                        if(particleAPISession.deviceList[data.coreid].online) {
                            sendConfiguration(data.coreid);
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
                        particleAPISession.deviceList[device.id].online = true;
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

const sendConfiguration = function (deviceId, newConfig = null) {
    if (particleAPISession.deviceList[deviceId].online) {
        api.getConfiguration(deviceId).then( (config) => {
            if (config.length > 0) {
                console.log(JSON.stringify(config));
                const configuration = config[0];
                if (newConfig) {
                    for(key in newConfig) {
                        configuration[key] = newConfig[key];
                    }
                }
                particle.callFunction({ deviceId: deviceId, name: 'configuration', argument: JSON.stringify(configuration), auth: particleAPISession.token }).then(
                    function (data) {
                        console.log(JSON.stringify(data));
                    },
                    function (error) {
                        console.log(error);
                        sendConfiguration(deviceId, configuration);
                    }
                );
            }
        });
    }
};