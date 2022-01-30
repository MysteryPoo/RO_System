import { Injectable } from '@nestjs/common';
import { databaseConstants } from 'src/database/constants';
import { DatabaseService } from 'src/database/database.service';

@Injectable()
export class UsersService {
  constructor(private databaseService: DatabaseService) {}

  async findOne(username: string): Promise<any> {
    const query = {
      username,
    };
    return this.databaseService.database
      .collection(databaseConstants.userCollectionName)
      .findOne(query);
  }

  async create(username: string, password: string): Promise<any> {
    const user = {
      username,
      password,
    };
    return this.databaseService.userCollection.insertOne(user);
  }
}
