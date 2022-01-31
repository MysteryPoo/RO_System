import { Controller, Get, Param, Query, UseGuards } from '@nestjs/common';
import { JwtAuthGuard } from 'src/auth/jwt-auth.guard';
import { StatesService } from 'src/database/states/states.service';

@Controller('devices/:deviceId/states')
export class StatesController {
  constructor(private stateService: StatesService) {}

  @UseGuards(JwtAuthGuard)
  @Get()
  async getStates(
    @Param('deviceId') deviceId: string,
    @Query('states') states: string,
    @Query('count') count: string,
    @Query('skip') skip: number,
    @Query('rows') rows: number,
  ) {
    try {
      const stateList: Array<string> = states
        ? JSON.parse(states)
        : ['IDLE', 'FILL', 'FLUSH'];
      return this.stateService.getStates(
        deviceId,
        stateList,
        count === 'true',
        Number(rows) || undefined,
        Number(skip) || undefined,
      );
    } catch (e) {
      console.error(e);
      return [];
    }
  }
}