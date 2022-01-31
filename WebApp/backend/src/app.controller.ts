import { Controller, Get } from '@nestjs/common';
import { ParticleService } from './particle/particle.service';

@Controller()
export class AppController {
  constructor(private particleService: ParticleService) {}

  @Get(['devices', 'deviceList'])
  getDevices() {
    return this.particleService.getDeviceList();
  }

  @Get('version')
  getVersion() {
    return process.env.npm_package_version;
  }
}
