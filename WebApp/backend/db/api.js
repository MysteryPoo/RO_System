const Joi = require('joi');
const { database, configCollection, statusCollection } = require('./connection');
const { ObjectId } = require('mongodb');
const particle = require('./particle');

class DeviceRequiredException {
    code = 422;
    message = "No device provided";
}

const eventSchema = Joi.object().keys({
    component: Joi.string().required(),
    datetime: Joi.date().required(),
    data: Joi.object().required()
});

async function getDeviceList(userId) {
    return particle.getDeviceList();
}

async function getDeviceStatus(deviceId) {
    const query = {
        deviceId
    };
    const deviceStatus = await statusCollection.findOne(query);
    return {
        online: deviceStatus.online,
    };
}

async function storeMessage(device, message) {
    const result = eventSchema.validate(message.data);
    if(result.error == null) {
        const collection = database.collection(device);
        return collection.insertOne(message.data);
    } else {
        return Promise.reject(result.error);
    }
}

async function getConfiguration(device) {
    const query = {
        deviceId: device,
    };
    const configuration = configCollection.findOne(query);
    return configuration;
}

async function setConfiguration(device, data) {
    if(device === null) {
        throw new DeviceRequiredException();
    }
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
    return configCollection.updateOne(query, update, options);
}

async function getTicks(
    device,
    skip = 0,
    rows = 1,
) {
    const collection = database.collection(device);
    const query = {
        component: 'system/tick',
    };
    const options = {
        sort: {
            datetime: -1,
        },
        limit: rows,
        skip,
    };
    const cursor = collection.find(query, options);
    const resultsArray = await cursor.toArray();
    return resultsArray;
}

async function getTicksByDate(
    device,
    dateTo = new Date(),
    dateFrom = new Date(dateTo.getTime() - (7 * 24 * 60 * 60 * 1000)),
    resolution = 10,
) {
    const collection = database.collection(device);
    const returnList = [];
    try{
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
        };
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
}

async function getLog(device, projectionOnly = false, rowCount = 10, skip = 0) {
    const collection = database.collection(device);
    const query = {
        component: {
            $in: [
                'ERROR',
                'TRACE',
                'INFO',
                'WARN',
                'system/restart',
            ],
        },
    };
    const options = {
        sort: {
            datetime: -1,
        },
        limit: rowCount,
        skip,
    };
    if (projectionOnly) {
        const cursor = await collection.countDocuments(query);
        return cursor;
    } else {
        const cursor = collection.find(query, options);
        const resultsArray = await cursor.toArray();
        return resultsArray;
    }
}

async function clearLog(device, log) {
    try {
        const collection = database.collection(device);
        const query = {
            '_id': ObjectId(log),
        };
        return collection.deleteOne(query);
    } catch (err) {
        console.log(err);
        return Promise.reject(Error("Failed to clear log message."));
    }
}

async function getPumpStates(device) {
    const collection = database.collection(device);
    const query = {
        component: 'relay/set',
        'data.name': 'COMPONENT_PUMP',
    };
    const options = {
        sort: {
            datetime: -1,
        },
        limit: 20,
    };
    const cursor = collection.find(query, options);
    const resultsArray = await cursor.toArray();
    return resultsArray;
}

async function getSystemStates(device, include = ['IDLE', 'FILL', 'FLUSH'], projectionOnly = false, rowCount = 10, skip = 0) {
    const collection = database.collection(device);
    const query = {
        component: 'system/state-request',
        'data.success': true,
        'data.state': {
            $in: include
        },
    };
    const options = {
        sort: {
            datetime: -1,
        },
        limit: projectionOnly ? null : rowCount,
        skip,
    };
    if (projectionOnly) {
        const cursor = await collection.countDocuments(query);
        return cursor;
    } else {
        const cursor = collection.find(query, options);
        const resultsArray = await cursor.toArray();
        return resultsArray;
    }
}

async function getRestarts(device) {
    const collection = database.collection(device);
    const query = {
        component: 'system/restart',
    };
    const options = {
        sort: {
            datetime: -1,
        },
        limit: 20,
    };
    const cursor = collection.find(query, options);
    const resultsArray = await cursor.toArray();
    return resultsArray;
}
 
module.exports = {
    getDeviceList,
    getDeviceStatus,
    setConfiguration,
    getConfiguration,
    storeMessage,
    getTicks,
    getTicksByDate,
    getLog,
    clearLog,
    getPumpStates,
    getSystemStates,
    getRestarts,
};
