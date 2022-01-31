import { Injectable } from '@nestjs/common';
import { Collection } from 'mongodb';
import { DatabaseService } from 'src/database/database.service';

@Injectable()
export class ConfigurationService {
  private collection: Collection;

  constructor(private databaseService: DatabaseService) {
    this.collection = this.databaseService.configCollection;
  }

  async getConfiguration(deviceId: string): Promise<any> {
    const query = {
      deviceId,
    };
    const configuration = await this.collection.findOne(query);
    return configuration?.configuration;
  }

  async setConfiguration(deviceId: string, data: any): Promise<void> {
    const previousConfiguration = await this.getConfiguration(deviceId);
    const newConfiguration = previousConfiguration || {};
    for (const key in data) {
      newConfiguration[key] = data[key];
    }
    const query = {
      deviceId,
    };
    const update = {
      $set: {
        deviceId,
        configuration: newConfiguration,
      },
    };
    const options = {
      upsert: true,
    };
    await this.collection.updateOne(query, update, options);
  }
}
