const monk = require('monk'); // TODO: Remove this requirement, move over to the mongodb driver completely.
const { MongoClient } = require("mongodb");
const connectionString = process.env.MONGODB_URI;
const db = monk(connectionString);
const client = new MongoClient(`mongodb://${connectionString}?retryWrites=true`);
client.connect();
 
module.exports = {
    db,
    client,
};
