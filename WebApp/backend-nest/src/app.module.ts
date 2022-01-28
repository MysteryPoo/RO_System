import { Module } from '@nestjs/common';
import { AppController } from './app.controller';
import { AuthModule } from './auth/auth.module';
import { ParticleModule } from './particle/particle.module';
import { DevicesModule } from './devices/devices.module';

@Module({
  imports: [AuthModule, ParticleModule, DevicesModule],
  controllers: [AppController],
  providers: [],
})
export class AppModule {}
