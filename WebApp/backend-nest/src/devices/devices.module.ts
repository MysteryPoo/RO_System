import { Module } from '@nestjs/common';
import { DatabaseModule } from 'src/database/database.module';
import { DevicesConfigurationController } from './configuration/configuration.controller';
import { DevicesController } from './devices.controller';
import { LogsController } from './logs/logs.controller';

@Module({
  imports: [DatabaseModule],
  controllers: [
    DevicesController,
    DevicesConfigurationController,
    LogsController,
  ],
})
export class DevicesModule {}
