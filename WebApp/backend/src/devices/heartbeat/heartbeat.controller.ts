import { Controller, Get, Param, Query, UseGuards } from '@nestjs/common';
import { JwtAuthGuard } from 'src/auth/jwt-auth.guard';
import { HeartbeatService } from 'src/database/heartbeat/heartbeat.service';

@Controller(['devices/:deviceId/heartbeat', 'devices/:deviceId/ticks'])
export class HeartbeatController {
  constructor(private heartbeatService: HeartbeatService) {}

  @UseGuards(JwtAuthGuard)
  @Get()
  async getHeartbeats(
    @Param('deviceId') deviceId: string,
    @Query('skip') skipParam: number,
    @Query('rows') rows: number,
  ) {
    return this.heartbeatService.getHeartbeats(
      deviceId,
      Number(skipParam) || undefined,
      Number(rows) || undefined,
    );
  }

  @UseGuards(JwtAuthGuard)
  @Get('byDate')
  async getHeartbeatsByDate(
    @Param('deviceId') deviceId: string,
    @Query('to') to: string,
    @Query('from') from: string,
    @Query('resolution') resolution: number,
  ) {
    return this.heartbeatService.getHeartbeatsByDate(
      deviceId,
      new Date(to),
      new Date(from),
      Number(resolution) || undefined,
    );
  }
}
