import { Module } from '@nestjs/common';
import { ConfigurationService } from './configuration/configuration.service';
import { DatabaseService } from './database.service';
import { LogsService } from './logs/logs.service';
import { UsersService } from './users/users.service';

@Module({
  providers: [DatabaseService, ConfigurationService, UsersService, LogsService],
  exports: [DatabaseService, ConfigurationService, UsersService, LogsService],
})
export class DatabaseModule {}
