import { Injectable } from '@nestjs/common';
import { Collection } from 'mongodb';
import { DatabaseService } from 'src/database/database.service';
import * as mqtt from 'mqtt';

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

  async triggerOption(
    deviceId: string,
    component: string,
    option: string,
  ): Promise<{ success: boolean }> {
    const options = {
      clientId: 'backend',
      username: process.env.MQTT_USERNAME,
      password: process.env.MQTT_PASSWORD,
      clean: true,
    };
    const client = mqtt.connect('mqtt://rabbitmq', options);
    const message = {};
    message[option] = true;
    client.publish(
      `to/${deviceId}/${component}/configuration`,
      JSON.stringify(message),
    );
    return { success: true };
  }
}
