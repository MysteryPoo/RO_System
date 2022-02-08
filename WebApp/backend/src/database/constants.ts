import 'dotenv/config';

const databaseName = process.env.MONGODB_DB_NAME ?? 'test';
const username = process.env.MONGODB_USERNAME ?? 'admin';
const password = process.env.MONGODB_PASSWORD ?? 'admin';
const connectionString = process.env.MONGODB_URI ?? 'localhost';

export const databaseConstants = {
  connectionUri: `mongodb://${username}:${password}@${connectionString}?retryWrites=true&authSource=admin`,
  databaseName,
  configurationCollectionName: 'configuration',
  statusCollectionName: 'status',
  userCollectionName: 'users',
  featureCollectionName: 'feature',
};
