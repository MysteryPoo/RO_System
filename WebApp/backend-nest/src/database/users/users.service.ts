import { Injectable } from '@nestjs/common';
import { Collection } from 'mongodb';
import { DatabaseService } from '../database.service';

@Injectable()
export class UsersService {
  private collection: Collection;

  constructor(private databaseService: DatabaseService) {
    this.collection = this.databaseService.userCollection;
  }

  public async findUser(username: string): Promise<any> {
    const query = {
      username,
    };
    return this.collection.findOne(query);
  }
}
