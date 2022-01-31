import { Module } from '@nestjs/common';
import { DatabaseService } from './database.service';
import { RestartService } from './restarts/restart.service';
import { StatesService } from './states/states.service';

@Module({
  providers: [DatabaseService, StatesService, RestartService],
  exports: [DatabaseService, StatesService, RestartService],
})
export class DatabaseModule {}
