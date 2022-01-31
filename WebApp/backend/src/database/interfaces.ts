import { Document as MongoDocument } from 'mongodb';

export interface IDeviceStatus extends MongoDocument {
  deviceId: string;
  last_heard: Date;
  online: boolean;
}
