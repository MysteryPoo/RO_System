import { Module } from '@nestjs/common';
import { ParticleService } from './particle.service';

@Module({
  providers: [ParticleService],
  exports: [ParticleService],
})
export class ParticleModule {}
