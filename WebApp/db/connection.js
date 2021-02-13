const monk = require('monk');
const connectionString = process.env.MONGODB_URI;
const db = monk(connectionString);
 
module.exports = db;
