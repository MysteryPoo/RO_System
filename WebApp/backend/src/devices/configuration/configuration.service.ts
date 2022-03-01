import { Injectable } from '@nestjs/common';
import { Collection } from 'mongodb';
import { DatabaseService } from 'src/database/database.service';
import { MqttService } from 'src/mqtt/mqtt.service';

@Injectable()
export class ConfigurationService {
  private collection: Collection;

  constructor(
    private databaseService: DatabaseService,
    private mqttService: MqttService,
  ) {
    this.collection = this.databaseService.configCollection;
  }

  async getConfiguration(deviceId: string): Promise<any> {
    const query = {
      deviceId,
    };
    const configuration = await this.collection.findOne(query);
    return configuration?.configuration;
  }

  async setConfiguration(
    deviceId: string,
    data: any,
  ): Promise<{ success: boolean; message: string }> {
    try {
      const previousConfiguration = await this.getConfiguration(deviceId);
      const newConfiguration = previousConfiguration || {};
      for (const component in data) {
        newConfiguration[component] = data[component];
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
    } catch (err) {
      return {
        success: false,
        message: 'Failed to save configuration.',
      };
    }
    for (const component in data) {
      await this.mqttService.publish(
        `to/${deviceId}/configuration/${component}`,
        JSON.stringify(data[component]),
      );
    }
    return {
      success: true,
      message: '',
    };
  }

  async triggerOption(
    deviceId: string,
    component: string,
    option: string,
  ): Promise<{ success: boolean }> {
    const message = {};
    message[option] = true;
    await this.mqttService.publish(
      `to/${deviceId}/configuration/${component}`,
      JSON.stringify(message),
    );
    return { success: true };
  }
}
