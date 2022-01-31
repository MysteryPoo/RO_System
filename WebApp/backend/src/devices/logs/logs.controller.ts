import {
  Controller,
  Delete,
  Get,
  Param,
  Query,
  UseGuards,
} from '@nestjs/common';
import { JwtAuthGuard } from 'src/auth/jwt-auth.guard';
import { LogsService } from 'src/devices/logs/logs.service';

@Controller('devices/:deviceId/logs')
export class LogsController {
  constructor(private logsService: LogsService) {}

  @UseGuards(JwtAuthGuard)
  @Get()
  async getLogs(
    @Param('deviceId') deviceId: string,
    @Query() query,
  ): Promise<Array<any> | number> {
    const skipParam = query.skip ? Number(query.skip) : undefined;
    const rowCount = query.rows ? Number(query.rows) : undefined;
    return this.logsService.getLogs(
      deviceId,
      query.count === 'true',
      rowCount,
      skipParam,
    );
  }

  @UseGuards(JwtAuthGuard)
  @Delete(':logId')
  async deleteLog(
    @Param('deviceId') deviceId: string,
    @Param('logId') logId: string,
  ) {
    return this.logsService.deleteLog(deviceId, logId);
  }
}
