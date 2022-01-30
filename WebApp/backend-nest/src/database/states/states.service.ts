import { Injectable } from '@nestjs/common';
import { FindOptions } from 'mongodb';
import { DatabaseService } from '../database.service';

@Injectable()
export class StatesService {
  constructor(private databaseService: DatabaseService) {}

  async getStates(
    deviceId: string,
    include = ['IDLE', 'FILL', 'FLUSH'],
    projectionOnly = false,
    rowCount = 10,
    skip = 0,
  ) {
    const collection = this.databaseService.database.collection(deviceId);
    const query = {
      component: 'system/state-request',
      'data.state': {
        $in: include,
      },
    };
    const options: FindOptions = {
      sort: {
        datetime: -1,
      },
      limit: projectionOnly ? null : rowCount,
      skip,
    };
    if (projectionOnly) {
      return collection.countDocuments(query);
    } else {
      const cursor = collection.find(query, options);
      return cursor.toArray();
    }
  }
}
