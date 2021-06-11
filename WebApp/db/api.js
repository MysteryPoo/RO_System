const Joi = require('joi');
const { db, client } = require('./connection');
const { ObjectId } = require('mongodb');

const eventSchema = Joi.object().keys({
    component: Joi.string().required(),
    datetime: Joi.date().required(),
    data: Joi.object().required()
});

const configuration = db.get('configuration');
const deviceList = [];

function storeMessage(device, message) {
    const result = eventSchema.validate(message.data);
    if(result.error == null) {
        return deviceList[device].insert(message.data);
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

async function setConfiguration(device, data) {
    if(device) {
        const database = client.db('test');
        const collection = database.collection('configuration');
        const query = {
            deviceId: device,
        };
        const update = {
            $set: {
                deviceId: device,
                enabled: data.enabled,
                fillDistances: {
                    start: data.fillDistances.start,
                    stop: data.fillDistances.stop,
                },
                sonicHeight: data.sonicHeight,
                floatHeight: data.floatHeight,
                diameter: data.diameter,
                pumpCooldown: data.pumpCooldown,
                tickRate: data.tickRate,
            },
        };
        const options = {
            upsert: true,
        };
        return collection.updateOne(query, update, options);
    }
    return Promise.reject(Error("Invalid device selected."));
}

async function getLastTick(
    device,
    dateTo = new Date(),
    dateFrom = new Date(dateTo.getTime() - (7 * 24 * 60 * 60 * 1000)),
    resolution = 10
) {
    if(deviceList[device]) {
        const returnList = [];
        try{
            const database = client.db('test');
            const collection = database.collection(device);
            const query = {
                component: 'system/tick',
                datetime: {
                    $gt: dateFrom,
                    $lt: dateTo,
                },
            };
            const options = {
                sort: {
                    datetime: -1,
                },
            }
            const recordCount = await collection.countDocuments(query, options);
            const interval = Math.max(1, Math.round(recordCount / resolution));
            for(let i = 0; i < recordCount; i += interval) {
                const cursor = collection.find(query, options).skip(i).limit(1);
                const tickList = await cursor.toArray();
                if(tickList.length > 0) {
                    returnList.push(tickList[0]);
                }
            }
        } catch (err) {
            console.log(err);
        }
        return returnList.reverse();
    } else {
        return Promise.reject(Error("Device does not exist."));
    }
}

function getLog(device) {
    if(deviceList[device]) {
        return deviceList[device].find({'component': { $in: ['ERROR', 'TRACE', 'INFO'] }}, {sort: {datetime: -1}, limit: 20});
    } else {
        return Promise.reject(Error("Device does not exist."));
    }
}

async function clearLog(device, log) {
    if(deviceList[device]) {
        try {
            const database = client.db('test');
            const collection = database.collection(device);
            const query = {
                '_id': ObjectId(log),
            };
            return collection.deleteOne(query);
        } catch (err) {
            console.log(err);
        }
    } else {
        return Promise.reject(Error("Device does not exist."));
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
    getLog,
    clearLog,
    getCurrentState,
    getPumpStates,
};
