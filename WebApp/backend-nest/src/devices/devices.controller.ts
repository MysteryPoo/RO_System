import { Controller, Get, Param, Query, UseGuards } from '@nestjs/common';
import { JwtAuthGuard } from 'src/auth/jwt-auth.guard';
import { DatabaseService } from 'src/database/database.service';

@Controller('devices/:deviceId')
export class DevicesController {
  constructor(private databaseService: DatabaseService) {}
}
