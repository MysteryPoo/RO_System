import { Injectable } from '@nestjs/common';
import { UsersService as DatabaseUsersService } from 'src/database/users/users.service';

@Injectable()
export class UsersService {
  constructor(private usersService: DatabaseUsersService) {}

  async findOne(username: string): Promise<any> {
    return this.usersService.findUser(username);
  }
}
