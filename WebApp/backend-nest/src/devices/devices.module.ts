import { Module } from '@nestjs/common';
import { DatabaseModule } from 'src/database/database.module';
import { DevicesConfigurationController } from './configuration/configuration.controller';
import { DevicesController } from './devices.controller';
import { HeartbeatController } from './heartbeat/heartbeat.controller';
import { LogsController } from './logs/logs.controller';
import { RestartController } from './restart/restart.controller';
import { StatesController } from './states/states.controller';
import { StatusController } from './status/status.controller';

@Module({
  imports: [DatabaseModule],
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
