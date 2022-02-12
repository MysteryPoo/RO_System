import { Body, Controller, Get, Param, Post, UseGuards } from '@nestjs/common';
import { JwtAuthGuard } from 'src/auth/jwt-auth.guard';
import { ConfigurationService } from './configuration.service';

@Controller('devices/:deviceId/configuration')
export class DevicesConfigurationController {
  constructor(private configurationService: ConfigurationService) {}

  @UseGuards(JwtAuthGuard)
  @Get()
  async getConfiguration(@Param('deviceId') deviceId: string): Promise<any> {
    return (await this.configurationService.getConfiguration(deviceId)) || {};
  }

  @UseGuards(JwtAuthGuard)
  @Post()
  async setConfiguration(
    @Param('deviceId') deviceId: string,
    @Body() body: any,
  ): Promise<{ success: boolean; message: string }> {
    return this.configurationService.setConfiguration(deviceId, body);
  }

  @UseGuards(JwtAuthGuard)
  @Get('trigger/:component/:option')
  async triggerOption(
    @Param('deviceId') deviceId: string,
    @Param('component') component: string,
    @Param('option') option: string,
  ): Promise<{ success: boolean }> {
    return this.configurationService.triggerOption(deviceId, component, option);
  }
}
