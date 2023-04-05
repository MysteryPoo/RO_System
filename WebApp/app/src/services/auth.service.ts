
export class AuthService {
  private _endPoint: string;

  constructor() {
    this._endPoint = process.env.API_URL ?? 'http://localhost:3000';    
  }

  Login(username: string, password: string): Promise<Response> {
    return fetch(`${this._endPoint}/auth/login`, {
        method: 'POST',
        headers: {
          Accept: 'application/json',
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          username,
          password,
        }),
    });
  }
}

export function useAuthService(): AuthService {
  return new AuthService()
}