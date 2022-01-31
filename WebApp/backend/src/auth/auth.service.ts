import { Injectable, HttpException, HttpStatus } from '@nestjs/common';
import { UsersService } from 'src/users/users.service';
import { JwtService } from '@nestjs/jwt';
import * as bcrypt from 'bcrypt';

@Injectable()
export class AuthService {
  constructor(
    private usersService: UsersService,
    private jwtService: JwtService,
  ) {}

  async validateUser(username: string, pass: string): Promise<any> {
    const user = await this.usersService.findOne(username);
    if (!user) {
      return null;
    }

    const validate = await bcrypt.compare(pass, user.password);
    if (!validate) {
      return null;
    }

    // eslint-disable-next-line @typescript-eslint/no-unused-vars
    const { password, ...result } = user;
    return result;
  }

  async signup(username: string, password: string): Promise<any> {
    const user = await this.usersService.findOne(username);
    if (user) {
      throw new HttpException('Forbidden', HttpStatus.FORBIDDEN);
    }

    return this.usersService.create(username, await bcrypt.hash(password, 10));
  }

  async login(user: any) {
    const access_token_payload = { username: user.username, sub: user.userId };
    const refresh_token_payload = {
      username: user.username,
      sub: user.username,
    };
    return {
      access_token: this.jwtService.sign(access_token_payload),
      refresh_token: this.jwtService.sign(refresh_token_payload),
      expiry: 60 * 60,
    };
  }
}
