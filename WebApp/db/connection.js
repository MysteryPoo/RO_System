const { MongoClient } = require("mongodb");
const mongoose = require('mongoose');

const connectionString = process.env.MONGODB_URI;
const mongo = new MongoClient(`mongodb://${connectionString}?retryWrites=true`, {
    useUnifiedTopology: true,
});
const databaseName = 'test';

mongoose.connect(`mongodb://${connectionString}/${databaseName}`, {
    useNewUrlParser: true,
    useUnifiedTopology: true,
});
mongoose.set('useCreateIndex', true);
mongoose.connection.on('error', err => console.log(err));
mongoose.Promise = global.Promise;

mongo.connect();
const database = mongo.db(databaseName);
const configCollection = database.collection('configuration');
 
module.exports = {
    database,
    configCollection,
};
