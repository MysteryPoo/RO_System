import { Module } from '@nestjs/common';
import { DatabaseService } from './database.service';
import { LogsService } from './logs/logs.service';
import { RestartService } from './restarts/restart.service';
import { StatesService } from './states/states.service';

@Module({
  providers: [DatabaseService, LogsService, StatesService, RestartService],
  exports: [DatabaseService, LogsService, StatesService, RestartService],
})
export class DatabaseModule {}
