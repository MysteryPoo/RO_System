const Joi = require('joi');
const db = require('./connection');

const eventSchema = Joi.object().keys({
    component: Joi.string().required(),
    datetime: Joi.date().required(),
    data: Joi.object().required()
});

const configuration = db.get('configuration');
const deviceList = [];
const events = db.get('events');

function storeMessage(device, message) {
    const result = eventSchema.validate(message.data);
    if(result.error == null) {
        return deviceList[device].insert(message.data);
        //return events.insert(message.data);
    } else {
        return Promise.reject(result.error);
    }
}

function addDevice(deviceName) {
    deviceList[deviceName] = db.get(deviceName);
}

function getConfiguration(device) {
    return configuration.find({'deviceId': device});
}

function setConfiguration(device, data) {
    return configuration.findOneAndUpdate({deviceId: device}, { $set:
        {
            enabled: data.enabled,
            fillStart: data.fillStart,
            fillStop: data.fillStop,
            sonicHeight: data.sonicHeight,
            floatHeight: data.floatHeight,
            diameter: data.diameter
        }
    });
}

function getLastTick(device) {
    if(deviceList[device]) {
        return deviceList[device].find({'component': 'system/tick'}, {sort: {datetime: -1}, limit: 500});
    } else {
        return [{
            data: {
                state: 'UNKNOWN'
            }
        }];
    }
}

function getCurrentState(device) {
    if(deviceList[device]) {
        return deviceList[device].find({'component': 'system/state-request', 'data.success': true}, {sort: {datetime: -1}, limit: 20});
    } else {
        return [{
            data: {
                state: 'UNKNOWN'
            }
        }];
    }
}

function getPumpStates(device) {
    return deviceList[device].find({'component': 'relay/set', 'data.name': 'COMPONENT_PUMP'}, {sort: {datetime: -1}, limit: 20});
}
 
module.exports = {
    addDevice,
    setConfiguration,
    getConfiguration,
    storeMessage,
    getLastTick,
    getCurrentState,
    getPumpStates,
};
