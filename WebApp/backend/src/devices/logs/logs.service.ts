import { Injectable } from '@nestjs/common';
import { Collection, FindOptions, ObjectId } from 'mongodb';
import { DatabaseService } from 'src/database/database.service';

@Injectable()
export class LogsService {
  constructor(private databaseService: DatabaseService) {}

  async getLogs(
    deviceId: string,
    projectionOnly = false,
    rowCount = 10,
    skip = 0,
  ): Promise<Array<any> | number> {
    const collection: Collection =
      this.databaseService.database.collection(deviceId);
    const query = {
      component: {
        $in: ['ERROR', 'TRACE', 'INFO', 'WARN', 'system/restart'],
      },
    };
    const options: FindOptions = {
      sort: {
        datetime: -1,
      },
      limit: rowCount,
      skip,
    };
    if (projectionOnly) {
      const cursor = await collection.countDocuments(query);
      return cursor;
    } else {
      const cursor = collection.find(query, options);
      return await cursor.toArray();
    }
  }

  async deleteLog(deviceId: string, logId: string) {
    const collection: Collection =
      this.databaseService.database.collection(deviceId);
    const query = {
      _id: new ObjectId(logId),
    };
    return collection.deleteOne(query);
  }
}
