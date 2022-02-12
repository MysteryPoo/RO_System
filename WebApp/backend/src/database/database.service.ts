import { Injectable } from '@nestjs/common';
import { Collection, Db, MongoClient } from 'mongodb';
import { databaseConstants } from './constants';

@Injectable()
export class DatabaseService {
  public database: Db;
  public configCollection: Collection;
  public statusCollection: Collection;
  public userCollection: Collection;
  public featureCollection: Collection;

  private mongo: MongoClient;

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
    this.featureCollection = this.database.collection(
      databaseConstants.featureCollectionName,
    );
    // Set up indices
    this.configCollection.createIndex({ deviceId: 1 }, { unique: true });
    this.statusCollection.createIndex({ deviceId: 1 }, { unique: true });
    this.userCollection.createIndex({ username: 1 }, { unique: true });
    this.featureCollection.createIndex({ component: 1 }, { unique: true });
  }
}
