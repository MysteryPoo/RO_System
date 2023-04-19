import { Session, User } from '@supabase/supabase-js';
import { defineStore } from 'pinia';

export const useAuthStore = defineStore('auth', {
  state: () => ({
    user: undefined as User | null | undefined,
    session: undefined as Session | null | undefined,
  }),
  getters: {},
  actions: {
    
  },
});
