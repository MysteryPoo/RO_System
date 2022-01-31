import { Controller, Get, Param, UseGuards } from '@nestjs/common';
import { JwtAuthGuard } from 'src/auth/jwt-auth.guard';
import { RestartService } from './restart.service';

@Controller('devices/:deviceId/restart')
export class RestartController {
  constructor(private restartService: RestartService) {}

  @UseGuards(JwtAuthGuard)
  @Get()
  async getRestarts(@Param('deviceId') deviceId: string) {
    return this.restartService.getRestarts(deviceId);
  }
}
