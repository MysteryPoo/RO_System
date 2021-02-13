const Joi = require('joi');
const db = require('./connection');

const eventSchema = Joi.object().keys({
    component: Joi.string().required(),
    datetime: Joi.date().required(),
    data: Joi.object().required()
})
 
const messages = db.get('misc');
const events = db.get('events');
 
function getAll() {
    return messages.find();
}

function storeMessage(event, message) {
    const result = eventSchema.validate(message.data);
    if(result.error == null) {
        return events.insert(message.data);
    } else {
        return Promise.reject(result.error);
    }
}

function getLastTick() {
    return events.find({'component': 'system/tick'}, {sort: {datetime: -1}, limit: 20});
}

function getCurrentState() {
    return events.find({'component': 'system/state-request', 'data.success': true}, {sort: {datetime: -1}, limit: 1});
}

function getPumpStates() {
    return events.find({'component': 'relay/set', 'data.name': 'COMPONENT_PUMP'}, {sort: {datetime: -1}, limit: 20});
}
 
module.exports = {
    getAll,
    storeMessage,
    getLastTick,
    getCurrentState,
    getPumpStates,
};
