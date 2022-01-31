import { Module } from '@nestjs/common';
import { DatabaseService } from './database.service';
import { StatesService } from './states/states.service';

@Module({
  providers: [DatabaseService, StatesService],
  exports: [DatabaseService, StatesService],
})
export class DatabaseModule {}
