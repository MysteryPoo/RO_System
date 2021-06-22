// DotENV setup
require('dotenv').config();

// Core includes
const express = require('express');
const cors = require('cors');
const morgan = require('morgan');

// API includes
const api = require('./db/api');

// Passport
const passport = require('passport');
require('./passport-config');

// Global objects
const app = express();

const authenticationRoute = require('./routes/authentication');
const userRoute = require('./routes/user');
const devicesRoute = require('./routes/devices');
 
app.use(morgan('tiny'));
app.use(cors());
app.use(express.json());
app.use(express.urlencoded({extended: false}));
 
app.use('/auth', authenticationRoute);
app.use('/user', passport.authenticate('jwt', {session: false}), userRoute);
app.use('/devices', passport.authenticate('jwt', {session: false}), devicesRoute);

app.get('/deviceList', async (req, res) => {
    res.json(await api.getDeviceList(null));
});

// Handle errors
app.use(function(err, req, res, next) {
    res.status(err.status || 500);
    res.json({error: err});
});

// Start listening
const port = process.env.PORT || 4000;
app.listen(port, () => {
    console.log(`listening on ${port}`);
});
