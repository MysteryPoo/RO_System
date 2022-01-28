import { Controller, Get } from '@nestjs/common';
import { ParticleService } from './particle/particle.service';

@Controller()
export class AppController {
  constructor(private particleService: ParticleService) {}

  @Get('devices')
  getDevices() {
    return this.particleService.getDeviceList();
  }
}
