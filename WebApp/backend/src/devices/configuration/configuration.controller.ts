import { Body, Controller, Get, Param, Post, UseGuards } from '@nestjs/common';
import { JwtAuthGuard } from 'src/auth/jwt-auth.guard';
import { ConfigurationService } from 'src/database/configuration/configuration.service';

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
  ): Promise<{ success: boolean; configuration: any }> {
    await this.configurationService.setConfiguration(deviceId, body);
    return { success: true, configuration: body };
  }
}
