import { Injectable } from '@nestjs/common';
import { particleConstants } from './constants';
import * as Particle from 'particle-api-js';

interface LoginResponse {
  token_type: string;
  access_token: string;
  expires_in: number;
  refresh_token: string;
}

interface LoginRequest {
  body: LoginResponse;
  statusCode: number;
}

interface Device {
  id: string;
  name: string;
  last_ip_address: string;
  last_heard: string;
  last_handshake_at: string;
  product_id: number;
  online: boolean;
  connected: boolean;
  platform_id: number;
  cellular: boolean;
  notes: unknown;
  functions: Array<unknown>;
  variables: object;
  status: string;
  serial_number: string;
  system_firmware_version: string;
  current_build_target: string;
  pinned_build_target: string;
  default_build_target: string;
}

interface IDeviceListRequest {
  body: Array<Device>;
  statusCode: number;
}

@Injectable()
export class ParticleService {
  private particle: any;

  constructor() {
    this.particle = new Particle();
  }

  async login(): Promise<string> {
    try {
      const loginRequest: LoginRequest = await this.particle.login({
        username: particleConstants.username,
        password: particleConstants.password,
      });
      if (loginRequest.statusCode !== 200) {
        return '';
      }
      return loginRequest.body.access_token;
    } catch (e) {
    } finally {
    }
  }

  async getDeviceList(): Promise<Array<any>> {
    const token = await this.login();
    const request: IDeviceListRequest = await this.particle.listDevices({
      auth: token,
    });
    const deviceList = [];
    Object.keys(request.body).forEach((key) => {
      const device = request.body[key];
      deviceList.push({
        id: device.id,
        name: device.name,
      });
    });
    return deviceList;
  }
}
