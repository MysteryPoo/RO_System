// DotENV setup
require('dotenv').config();

const express = require('express');
const cors = require('cors');
const bodyParser = require('body-parser');
const morgan = require('morgan');

const Particle = require('particle-api-js');
const messages = require('./db/messages');
 
const particle = new Particle();
const app = express();

const particleAPISession = {
    client_id: process.env.PARTICLE_CLIENT_ID,
    client_secret: process.env.PARTICLE_CLIENT_SECRET,
    username: process.env.PARTICLE_USERNAME,
    password: process.env.PARTICLE_PASSWORD,
    token: undefined,
    expires_in: undefined,
    refresh_token: undefined,
    valid: false,
    event_stream: null,
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

app.get('/lastTick', (req, res) => {
    messages.getLastTick().then( (data) => {
        res.json(data);
    });
});

app.get('/currentState', (req, res) => {
    messages.getCurrentState().then( (data) => {
        res.json(data);
    });
});

app.get('/pumpState', (req, res) => {
    messages.getPumpStates().then( (data) => {
        res.json(data);
    });
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

        particle.getEventStream({ deviceId: particleAPISession.deviceId, name: "romcon", auth: particleAPISession.token }).then(function (stream) {
            particleAPISession.event_stream = stream;
            stream.on('event', function (data) {
                const dataParsed = data;
                dataParsed.data = JSON.parse(dataParsed.data);
                if(dataParsed.data.datetime != 'undefined') {
                    dataParsed.data.datetime = new Date(dataParsed.data.datetime);
                } else {
                    dataParsed.data.datetime = new Date(Date(dataParsed.published_at));
                }
                console.log(dataParsed);
                messages.storeMessage(null, dataParsed);
            });
        });
    },
    function(err) {
        console.log(err);
    }
);
