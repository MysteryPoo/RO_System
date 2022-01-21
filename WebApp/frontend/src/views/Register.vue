<template>
    <div>
        <Dialog v-bind:visible="showSuccess" :breakpoints="{ '960px': '80vw' }" :style="{ width: '30vw' }" position="top">
            <div class="p-d-flex p-ai-center p-dir-col p-pt-6 p-px-3">
                <i class="pi pi-check-circle" :style="{fontSize: '5rem', color: 'var(--green-500)' }"></i>
                <h5>Registration Successful!</h5>
                <p :style="{lineHeight: 1.5, textIndent: '1rem'}">
                    Your account is registered under name <b>{{state.name}}</b>
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
                <h5>Registration Failed!</h5>
                <p :style="{lineHeight: 1.5, textIndent: '1rem'}">
                    
                </p>
            </div>
            <template #footer>
                <div class="p-d-flex p-jc-center">
                    <Button label="OK" @click="toggleError" class="p-button-text" />
                </div>
            </template>
        </Dialog>
        <div class="p-d-flex p-jc-center">
            <Card>
                <template #title> Register </template>
                <template #content>
                    <form @submit.prevent="handleSubmit(!v$.$invalid)" class="p-fluid">
                        <div class="p-field">
                            <div class="p-float-label">
                                <InputText id="name" v-model="v$.name.$model" :class="{'p-invalid':v$.name.$invalid && submitted}" />
                                <label for="name" :class="{'p-error':v$.name.$invalid && submitted}">Name*</label>
                            </div>
                            <small v-if="(v$.name.$invalid && submitted) || v$.name.$pending.$response" class="p-error">{{v$.name.required.$message.replace('Value', 'Name')}}</small>
                        </div>
                        <div class="p-field">
                            <div class="p-float-label">
                                <Password id="password" v-model="v$.password.$model" :class="{'p-invalid':v$.password.$invalid && submitted}" toggleMask>
                                    <template #header>
                                        <h6>Pick a password</h6>
                                    </template>
                                    <template #footer="sp">
                                        {{sp.level}}
                                        <Divider />
                                        <p class="p-mt-2">Suggestions</p>
                                        <ul class="p-pl-2 p-ml-2 p-mt-0" style="line-height: 1.5">
                                            <li>At least one lowercase</li>
                                            <li>At least one uppercase</li>
                                            <li>At least one numeric</li>
                                            <li>Minimum 8 characters</li>
                                        </ul>
                                    </template>
                                </Password>
                                <label for="password" :class="{'p-error':v$.password.$invalid && submitted}">Password*</label>
                            </div>
                            <small v-if="(v$.password.$invalid && submitted) || v$.password.$pending.$response" class="p-error">{{v$.password.required.$message.replace('Value', 'Password')}}</small>
                        </div>
                        <Button type="submit" label="Submit" class="p-mt-2" />
                    </form>
                </template>
            </Card>
        </div>
  </div>
</template>

<script setup lang="ts">
import { ref, reactive } from 'vue';
import { useRouter } from 'vue-router';
import useVuelidate from '@vuelidate/core';
import { required } from '@vuelidate/validators';
import InputText from 'primevue/inputtext';
import Divider from 'primevue/divider';
import Password from 'primevue/password';
import Button from 'primevue/button';
import Dialog from 'primevue/dialog';
import Card from 'primevue/card';

const state = reactive({
    name: '',
    password: '',
});

const rules = {
    name: { required },
    password: { required },
};

const v$ = useVuelidate(rules, state);

const router = useRouter();
const submitted = ref(false);
const showSuccess = ref(false);
const showError = ref(false);

const handleSubmit = async (isFormValid : boolean) => {
    submitted.value = true;
    if(!isFormValid) {
        return;
    }
    const registerRequest = await register();
    if(registerRequest.status === 200) {
        toggleDialog();
    } else {
        toggleError();
    }
};

const toggleDialog = () => {
    if(showSuccess.value) {
        router.push('/login');
    }
    showSuccess.value = !showSuccess.value;
};

const toggleError = () => {
    showError.value = !showError.value;
};

const register = () => {
    const formData = {
        username: state.name,
        password: state.password
    };
    return fetch(`http://${window.location.hostname}:4000/auth/signup`, {
        method: 'POST',
        headers: {
        Accept: 'application/json',
        'Content-Type': 'application/json',
        },
        body: JSON.stringify(formData),
    });
}
</script>
