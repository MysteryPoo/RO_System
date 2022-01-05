const { MongoClient } = require("mongodb");

const connectionString = process.env.MONGODB_URI ?? 'localhost';
const mongo = new MongoClient(`mongodb://${connectionString}?retryWrites=true`, {
    useUnifiedTopology: true,
});
const databaseName = process.env.MONGODB_DB_NAME ?? 'test';

mongo.connect();
const database = mongo.db(databaseName);
const configCollection = database.collection('configuration');
const statusCollection = database.collection('status');
 
module.exports = {
    database,
    configCollection,
    statusCollection,
};
