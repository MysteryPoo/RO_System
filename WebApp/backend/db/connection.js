const { MongoClient } = require("mongodb");

const databaseName = process.env.MONGODB_DB_NAME ?? 'test';
const username = process.env.MONGODB_USERNAME ?? 'admin';
const password = process.env.MONGODB_PASSWORD ?? 'admin';
const connectionString = process.env.MONGODB_URI ?? 'localhost';
const mongo = new MongoClient(`mongodb://${username}:${password}@${connectionString}?retryWrites=true&authSource=admin`, {
    useUnifiedTopology: true,
});

mongo.connect();
const database = mongo.db(databaseName);
const configCollection = database.collection('configuration');
const statusCollection = database.collection('status');

database.collection('configuration').createIndex({'deviceId': 1}, {unique: true});
database.collection('status').createIndex({'deviceId': 1}, {unique: true});
database.collection('users').createIndex({'username': 1}, {unique: true});
 
module.exports = {
    database,
    configCollection,
    statusCollection,
};
