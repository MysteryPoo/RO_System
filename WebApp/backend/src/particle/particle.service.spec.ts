import { Test, TestingModule } from '@nestjs/testing';
import { ParticleService } from './particle.service';

describe('ParticleService', () => {
  let service: ParticleService;

  beforeEach(async () => {
    const module: TestingModule = await Test.createTestingModule({
      providers: [ParticleService],
    }).compile();

    service = module.get<ParticleService>(ParticleService);
  });

  it('should be defined', () => {
    expect(service).toBeDefined();
  });
});
