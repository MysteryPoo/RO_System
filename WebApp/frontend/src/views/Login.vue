<template>
  <div class="surface-card p-4 shadow-2 border-round w-full lg:w-6">
    <Dialog v-bind:visible="showSuccess" :breakpoints="{ '960px': '80vw' }" :style="{ width: '30vw' }" position="top">
      <div class="p-d-flex p-ai-center p-dir-col p-pt-6 p-px-3">
        <i class="pi pi-check-circle" :style="{fontSize: '5rem', color: 'var(--green-500)' }"></i>
        <h5>Login Successful!</h5>
        <p :style="{lineHeight: 1.5, textIndent: '1rem'}">
          
        </p>
      </div>
      <template #footer>
        <div class="p-d-flex p-jc-center">
          <Button label="OK" @click="toggleDialog" class="p-button-text" />
        </div>
      </template>
    </Dialog>
    <Dialog v-bind:visible="showError" :breakpoints="{ '960px': '80vw' }" :style="{ width: '30vw' }" position="top">
      <div class="p-d-flex p-ai-center p-dir-col p-pt-6 p-px-3">
        <i class="pi pi-times-circle" :style="{fontSize: '5rem', color: '#DC143C' }"></i>
        <h5>Login Failed!</h5>
        <p :style="{lineHeight: 1.5, textIndent: '1rem'}">
          {{ errorMessage }}
        </p>
      </div>
      <template #footer>
        <div class="p-d-flex p-jc-center">
          <Button label="OK" @click="toggleError" class="p-button-text" />
        </div>
      </template>
    </Dialog>
    <div class="text-center mb-5">
      <!--<img src="images/blocks/logos/hyper.svg" alt="Image" height="50" class="mb-3">-->
      <div class="text-900 text-3xl font-medium mb-3">Welcome Back</div>
      <span class="text-600 font-medium line-height-3">Don't have an account?</span>
      <router-link :to="{path: '/register'}" class="font-medium no-underline ml-2 text-blue-500 cursor-pointer">Create today!</router-link>
    </div>

    <div>
      <form @submit.prevent="handleSubmit(!v$.$invalid)" class="p-fluid">
        <div class="p-field">
          <label for="name" class="block text-900 font-medium mb-2" :class="{'p-error':v$.name.$invalid && submitted}">Username</label>
          <InputText id="name" type="text" class="w-full mb-3" v-model="v$.name.$model" :class="{'p-invalid':v$.name.$invalid && submitted}"/>
          <small v-if="(v$.name.$invalid && submitted) || v$.name.$pending" class="p-error">{{v$.name.required.$message.replace('Value', 'Name')}}</small>
        </div>

        <div class="p-field">
          <label for="password" class="block text-900 font-medium mb-2" :class="{'p-error':v$.password.$invalid && submitted}">Password</label>
          <Password id="password" class="w-full mb-3" v-model="v$.password.$model" :class="{'p-invalid':v$.password.$invalid && submitted}" toggleMask/>
          <small v-if="(v$.password.$invalid && submitted) || v$.password.$pending" class="p-error">{{v$.password.required.$message.replace('Value', 'Password')}}</small>
        </div>

        <Button type="submit" label="Sign In" icon="pi pi-user" class="w-full"></Button>
      </form>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, reactive } from 'vue';
import { useRouter } from 'vue-router';
import { useStore } from 'vuex';
import useVuelidate from '@vuelidate/core';
import { required, alphaNum } from '@vuelidate/validators';
import InputText from 'primevue/inputtext';
import Password from 'primevue/password';
import Button from 'primevue/button';
import Dialog from 'primevue/dialog';

const state = reactive({
    name: '',
    password: '',
});

const rules = {
    name: { required, alphaNum },
    password: { required },
};

const v$ = useVuelidate(rules, state);

const router = useRouter();
const store = useStore();
const submitted = ref(false);
const showSuccess = ref(false);
const showError = ref(false);
const errorMessage = ref('');

const handleSubmit = async (isFormValid : boolean) => {
  submitted.value = true;
  if(!isFormValid) {
    return;
  }
  const loginRequest = await login();
  if(loginRequest.status === 201) {
    const response = await loginRequest.json();
    if(response.access_token) {
      toggleDialog();
      window.localStorage.token = response.access_token;
      store.commit('set_token', response.access_token);
    } else if(response.message) {
      displayError(response.message);
    } else {
      displayError(response.message);
    }
  } else {
    displayError(`Server returned an error: ${loginRequest.status}`);
  }
};

const toggleDialog = () => {
  if(showSuccess.value) {
    router.push('/');
  }
  showSuccess.value = !showSuccess.value;
}

const toggleError = () => {
    showError.value = !showError.value;
};

const login = () => {
    const formData = {
        username: state.name,
        password: state.password
    };
    return fetch(`http://${window.location.hostname}:4000/auth/login`, {
        method: 'POST',
        headers: {
          Accept: 'application/json',
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(formData),
    });
};

const displayError = (message : string) => {
  errorMessage.value = message;
  window.localStorage.token = undefined;
  store.commit('set_token', undefined);
  toggleError();
}
</script>
