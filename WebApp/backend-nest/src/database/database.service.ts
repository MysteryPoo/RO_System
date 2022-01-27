import { Injectable } from '@nestjs/common';
import { Collection, Db, MongoClient } from 'mongodb';
import { databaseConstants } from './constants';

@Injectable()
export class DatabaseService {
  private mongo: MongoClient;
  private database: Db;
  private configCollection: Collection;
  private statusCollection: Collection;
  private userCollection: Collection;

  constructor() {
    // Create connection
    this.mongo = new MongoClient(databaseConstants.connectionUri);
    this.mongo.connect();
    // Set up database
    this.database = this.mongo.db(databaseConstants.databaseName);
    this.configCollection = this.database.collection(
      databaseConstants.configurationCollectionName,
    );
    this.statusCollection = this.database.collection(
      databaseConstants.statusCollectionName,
    );
    this.userCollection = this.database.collection(
      databaseConstants.userCollectionName,
    );
    // Set up indices
    this.configCollection.createIndex({ deviceId: 1 }, { unique: true });
    this.statusCollection.createIndex({ deviceId: 1 }, { unique: true });
    this.userCollection.createIndex({ username: 1 }, { unique: true });
  }

  public async findUser(username: string): Promise<any> {
    const collection = this.userCollection;
    const query = {
      username,
    };
    return collection.findOne(query);
  }
}
