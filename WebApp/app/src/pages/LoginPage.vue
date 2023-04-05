<template>
  <q-page class="column items-center">
    <q-card class="q-ma-lg">
      <q-card-section>
        <div class="text-h1">Romcon</div>
      </q-card-section>
      <q-card-section>
        <div class="text-h6">Please login to access your device.</div>
      </q-card-section>
      <q-card-section>
        <q-form
          @submit="login">
          <q-input
            ref="usernameRef"
            class="q-mb-md"
            v-model="username"
            :disable="isWaiting"
            filled
            label="Username"
            debounce="500"
            autocorrect="off"
            autocapitalize="off"
            autocomplete="off"
            spellcheck="false"
            :rules="[val => !!val || 'Field is required']">
            <template v-slot:append>
              <q-avatar>
                <q-icon :name="matAccountCircle" />
              </q-avatar>
            </template>
          </q-input>
          <q-input
            ref="passwordRef"
            v-model="password"
            :disable="isWaiting"
            filled type="password"
            label="Password"
            debounce="500"
            autocorrect="off"
            autocapitalize="off"
            autocomplete="off"
            spellcheck="false"
            :rules="[val => !!val || 'Field is required']">
            <template v-slot:append>
              <q-avatar>
                <q-icon :name="matPassword" />
              </q-avatar>
            </template>
          </q-input>
          <q-btn type="submit" color="primary" :icon-right="matLogin" label="Sign In" :loading="isWaiting" />
        </q-form>
      </q-card-section>
    </q-card>
  </q-page>
</template>

<script setup lang="ts">
import { matAccountCircle, matPassword, matLogin } from '@quasar/extras/material-icons';
import { useAuthService, type AuthService } from '../services/auth.service';
import { ref, Ref } from 'vue';
import { QInput, useQuasar } from 'quasar';

const $q = useQuasar();
const authService: AuthService = useAuthService();

const username = ref('');
const usernameRef : Ref<QInput | null> = ref(null);
const password = ref('');
const passwordRef : Ref<QInput | null> = ref(null);

const isWaiting = ref(false);

async function login() : Promise<void> {
  usernameRef.value?.validate();
  passwordRef.value?.validate();

  if (usernameRef.value?.hasError || passwordRef.value?.hasError) {
    $q.notify({
      color: 'negative',
      message: 'Please enter a username and password.',
      position: 'top',
    });
    return;
  }

  try {
    isWaiting.value = true;
    const response: Response = await authService.Login(username.value, password.value);
    console.log(response);
  } catch (e) {
    
  } finally {
    isWaiting.value = false;
  }
}

</script>
