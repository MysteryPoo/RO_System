import { Injectable } from '@nestjs/common';
import { FindOptions } from 'mongodb';
import { DatabaseService } from '../database.service';

@Injectable()
export class RestartService {
  constructor(private databaseService: DatabaseService) {}

  async getRestarts(deviceId: string): Promise<Array<any>> {
    const collection = this.databaseService.database.collection(deviceId);
    const query = {
      component: 'system/restart',
    };
    const options: FindOptions = {
      sort: {
        datetime: -1,
      },
      limit: 20,
    };
    const cursor = collection.find(query, options);
    const resultsArray = await cursor.toArray();
    return resultsArray;
  }
}
