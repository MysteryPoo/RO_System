<template>
  <q-page class="column items-center">
    <q-card class="q-ma-lg">
      <q-card-section>
        <div class="text-h1">Sign Out</div>
      </q-card-section>
      <q-card-section>
        <div class="text-h6">Are you sure?</div>
      </q-card-section>
      <q-card-section>
        <q-btn color="primary" :icon-right="matCheck" label="Yes" @click="onClickYes" />
        <q-btn color="secondary" :icon-right="matClose" label="No" @click="onClickNo" />
      </q-card-section>
    </q-card>
  </q-page>
</template>

<script setup lang="ts">
import { supabase } from 'src/services/supabase.service';
import { matCheck, matClose } from '@quasar/extras/material-icons';
import { useRouter } from 'vue-router';

const router = useRouter();

function onClickYes() {
  supabase.auth.signOut();
  router.push('/auth');
}

function onClickNo() {
  router.push('/');
}

</script>
