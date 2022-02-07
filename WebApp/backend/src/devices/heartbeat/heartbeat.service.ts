import { Injectable } from '@nestjs/common';
import { Collection, FindOptions } from 'mongodb';
import { DatabaseService } from 'src/database/database.service';

@Injectable()
export class HeartbeatService {
  constructor(private databaseService: DatabaseService) {}

  async getStatus(deviceId: string): Promise<{ online: boolean }> {
    const collection = this.databaseService.statusCollection;
    const query = {
      deviceId,
    };
    const status = await collection.findOne(query);
    return {
      online: status.online,
    };
  }

  async getHeartbeats(deviceId: string, skip = 0, rows = 1) {
    const collection: Collection =
      this.databaseService.database.collection(deviceId);
    const query = {
      component: {
        $in: ['system/tick', 'system/heartbeat'],
      },
    };
    const options: FindOptions = {
      sort: {
        datetime: -1,
      },
      limit: rows,
      skip,
    };
    const cursor = collection.find(query, options);
    return cursor.toArray();
  }

  async getHeartbeatsByDate(
    deviceId: string,
    dateTo = new Date(),
    dateFrom = new Date(dateTo.getTime() - 7 * 24 * 60 * 60 * 1000),
    resolution = 10,
  ) {
    const collection = this.databaseService.database.collection(deviceId);
    const returnList = [];
    try {
      const query = {
        component: {
          $in: ['system/tick', 'system/heartbeat'],
        },
        datetime: {
          $gt: dateFrom,
          $lt: dateTo,
        },
      };
      const options: FindOptions = {
        sort: {
          datetime: -1,
        },
      };
      const recordCount = await collection.countDocuments(query, options);
      const interval = Math.max(1, Math.round(recordCount / resolution));
      for (let i = 0; i < recordCount; i += interval) {
        const cursor = collection.find(query, options).skip(i).limit(1);
        const tickList = await cursor.toArray();
        if (tickList.length > 0) {
          returnList.push(tickList[0]);
        }
      }
    } catch (err) {
      console.error(err);
    }
    return returnList.reverse();
  }

  async getFeatureList(deviceId: string): Promise<Array<string>> {
    const collection = this.databaseService.database.collection(deviceId);
    const query = {
      component: 'system/feature-list',
    };
    const options: FindOptions = {
      sort: {
        datetime: -1,
        limit: 1,
      },
    };
    const cursor = collection.find(query, options);
    const documents = await cursor.toArray();
    return documents[0].data.features;
  }
}
