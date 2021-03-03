const Joi = require('joi');
const { db, client } = require('./connection');

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

async function getLastTick(
    device,
    dateTo = new Date(),
    dateFrom = new Date((new Date()).getTime() - (7 * 24 * 60 * 60 * 1000)),
    resolution = 10
) {
    if(deviceList[device]) {
        console.log(dateTo.toLocaleString());
        console.log(dateFrom.toLocaleString());
        console.log(resolution);
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
        return returnList;
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
    getCurrentState,
    getPumpStates,
};
