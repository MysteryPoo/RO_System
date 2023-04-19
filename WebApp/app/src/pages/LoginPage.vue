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
            ref="emailRef"
            class="q-mb-md"
            v-model="email"
            :disable="isWaiting"
            filled
            type="email"
            label="Email"
            debounce="500"
            autocorrect="off"
            autocapitalize="off"
            autocomplete="off"
            spellcheck="false"
            :rules="[val => !!val || 'Field is required', val => isValidEmail(val) || 'Please enter a valid email address.']">
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
            filled
            :type="hidePassword ? 'password' : 'text'"
            label="Password"
            debounce="500"
            autocorrect="off"
            autocapitalize="off"
            autocomplete="off"
            spellcheck="false"
            :rules="[val => !!val || 'Field is required']">
            <template v-slot:append>
              <q-icon
                :name="hidePassword ? 'visibility_off' : 'visibility'"
                class="cursor-pointer"
                @click="hidePassword = !hidePassword"
              />
              <q-avatar>
                <q-icon :name="matPassword" />
              </q-avatar>
            </template>
          </q-input>
          <q-btn type="submit" color="primary" :icon-right="matLogin" label="Sign In" :loading="isWaiting" />
          <q-btn color="secondary" :icon-right="matLogin" label="Sign Up" :loading="isWaiting" @click="create" />
        </q-form>
      </q-card-section>
    </q-card>
  </q-page>
</template>

<script setup lang="ts">
import { useAuthStore } from '../stores/auth-store';
import { matAccountCircle, matPassword, matLogin } from '@quasar/extras/material-icons';
import { supabase } from 'src/services/supabase.service';
import { ref, Ref } from 'vue';
import { useRouter } from 'vue-router';
import { QInput, useQuasar } from 'quasar';
import { AuthResponse } from '@supabase/supabase-js';

const $q = useQuasar();
const $auth = useAuthStore();
const router = useRouter();

const email = ref('');
const emailRef : Ref<QInput | null> = ref(null);
const password = ref('');
const passwordRef : Ref<QInput | null> = ref(null);

const hidePassword = ref(true);
const isWaiting = ref(false);

async function login() : Promise<void> {
  validateForm();

  try {
    isWaiting.value = true;
    const response = await supabase.auth.signInWithPassword({
      email: email.value,
      password: password.value,
    });
    $auth.user = response?.data.user;
    $auth.session = response?.data.session;
    console.log(response);
  } catch (e) {
    console.log(e);
  } finally {
    isWaiting.value = false;
    if ($auth.session) {
      router.push('/');
    }
  }
}

async function create() : Promise<void> {
  validateForm();
  isWaiting.value = true;
  const response: AuthResponse | undefined = await supabase.auth.signUp({
    email: email.value,
    password: password.value,
  });
  if (response?.error) {
    $q.notify({
      type: 'error',
      message: response.error.message,
    });
  }
  console.log(response);
  $auth.user = response?.data.user;
  $auth.session = response?.data.session;
  isWaiting.value = false;
}

function validateForm() : boolean {
  emailRef.value?.validate();
  passwordRef.value?.validate();

  if (emailRef.value?.hasError || passwordRef.value?.hasError) {
    $q.notify({
      color: 'negative',
      message: 'Please enter a username and password.',
      position: 'top',
    });
    return false;
  }

  return true;
}

function isValidEmail(email: string): boolean {
  const emailPattern = /^(?=[a-zA-Z0-9@._%+-]{6,254}$)[a-zA-Z0-9._%+-]{1,64}@(?:[a-zA-Z0-9-]{1,63}\.){1,8}[a-zA-Z]{2,63}$/;
  return emailPattern.test(email);
}

</script>
