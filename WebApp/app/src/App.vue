<template>
  <router-view />
</template>

<script setup lang="ts">
import { provide } from 'vue';
import { supabase } from './services/supabase.service';

provide('supabase', supabase);
</script>
