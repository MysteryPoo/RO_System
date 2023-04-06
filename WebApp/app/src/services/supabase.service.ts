import { createClient, type SupabaseClient } from '@supabase/supabase-js'

export class SupabaseService {

  private _supabaseUrl = process.env.SUPABASE_URL ?? '';
  private _supabaseAnonKey = process.env.SUPABASE_ANON_KEY ?? '';
  private _client: SupabaseClient | null = null;

  constructor() {
    this._client = createClient(this._supabaseUrl, this._supabaseAnonKey);
  }

  public get Client() { return this._client; }
}
