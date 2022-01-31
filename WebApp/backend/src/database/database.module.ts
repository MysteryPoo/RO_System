import { Module } from '@nestjs/common';
import { DatabaseService } from './database.service';
import { HeartbeatService } from './heartbeat/heartbeat.service';
import { LogsService } from './logs/logs.service';
import { RestartService } from './restarts/restart.service';
import { StatesService } from './states/states.service';

@Module({
  providers: [
    DatabaseService,
    LogsService,
    HeartbeatService,
    StatesService,
    RestartService,
  ],
  exports: [
    DatabaseService,
    LogsService,
    HeartbeatService,
    StatesService,
    RestartService,
  ],
})
export class DatabaseModule {}
