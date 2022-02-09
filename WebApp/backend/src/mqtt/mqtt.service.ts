import { Injectable } from '@nestjs/common';
import * as mqtt from 'mqtt';
import { constants } from './constants';

@Injectable()
export class MqttService {
  private client: mqtt.MqttClient;
  private mqttOptions: mqtt.IClientOptions = {
    clientId: 'backend',
    username: constants.username,
    password: constants.password,
    clean: true,
  };

  constructor() {
    this.connect();
  }

  connect() {
    // Create connection
    this.client = mqtt.connect('mqtt://rabbitmq', this.mqttOptions);
  }

  publish(topic: string, payload: string): Promise<boolean> {
    return new Promise<boolean>((resolve, reject) => {
      this.client.publish(topic, payload, (err) => {
        if (err) {
          this.connect();
          reject(false);
        }
        resolve(true);
      });
    });
  }
}
