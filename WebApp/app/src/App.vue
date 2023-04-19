<template>
  <router-view />
</template>

<script setup lang="ts">
import { onMounted } from 'vue';
import { useRouter } from 'vue-router';
import { supabase } from './services/supabase.service';

const router = useRouter();

onMounted( () => {
  router.beforeEach( async (to) => {
    const { data, error } = await supabase.auth.getSession();
    const isValidSession = data && data.session != null && !error;
    if (!isValidSession && to.path !== '/auth') {
      return { path: '/auth' };
    }
    if (isValidSession && to.path === '/auth') {
      return { path: '' };
    }
  });
});
</script>
