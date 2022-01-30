import { Controller, Get, Param, UseGuards } from '@nestjs/common';
import { JwtAuthGuard } from 'src/auth/jwt-auth.guard';
import { HeartbeatService } from 'src/database/heartbeat/heartbeat.service';

@Controller('devices/:deviceId/status')
export class StatusController {
  constructor(private heartbeatService: HeartbeatService) {}

  @UseGuards(JwtAuthGuard)
  @Get()
  async getStatus(@Param('deviceId') deviceId: string) {
    return this.heartbeatService.getStatus(deviceId);
  }
}
