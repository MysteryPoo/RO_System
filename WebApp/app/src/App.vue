<template>
  <router-view />
</template>

<script setup lang="ts">
import { provide } from 'vue';
import { SupabaseService } from './services/supabase.service';

provide('supabase', new SupabaseService());
</script>
