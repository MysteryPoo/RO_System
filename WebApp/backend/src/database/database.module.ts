import { Module } from '@nestjs/common';
import { ConfigurationService } from './configuration/configuration.service';
import { DatabaseService } from './database.service';
import { HeartbeatService } from './heartbeat/heartbeat.service';
import { LogsService } from './logs/logs.service';
import { RestartService } from './restarts/restart.service';
import { StatesService } from './states/states.service';

@Module({
  providers: [
    DatabaseService,
    ConfigurationService,
    LogsService,
    HeartbeatService,
    StatesService,
    RestartService,
  ],
  exports: [
    DatabaseService,
    ConfigurationService,
    LogsService,
    HeartbeatService,
    StatesService,
    RestartService,
  ],
})
export class DatabaseModule {}
