import { Module } from '@nestjs/common';
import { DatabaseModule } from 'src/database/database.module';
import { DevicesConfigurationController } from './configuration/configuration.controller';
import { ConfigurationService } from './configuration/configuration.service';
import { DevicesController } from './devices.controller';
import { HeartbeatController } from './heartbeat/heartbeat.controller';
import { HeartbeatService } from './heartbeat/heartbeat.service';
import { LogsController } from './logs/logs.controller';
import { LogsService } from './logs/logs.service';
import { RestartController } from './restart/restart.controller';
import { RestartService } from './restart/restart.service';
import { StatesController } from './states/states.controller';
import { StatesService } from './states/states.service';
import { StatusController } from './status/status.controller';

@Module({
  imports: [DatabaseModule],
  providers: [
    ConfigurationService,
    HeartbeatService,
    LogsService,
    RestartService,
    StatesService,
  ],
  controllers: [
    DevicesController,
    DevicesConfigurationController,
    LogsController,
    HeartbeatController,
    StatusController,
    RestartController,
    StatesController,
  ],
})
export class DevicesModule {}
