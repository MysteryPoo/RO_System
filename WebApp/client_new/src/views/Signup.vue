<template>
  <div>
    <VeeForm v-slot="{ handleSubmit }" as="div">
      <form @submit="handleSubmit($event, onSubmit)">
        <Field name="username" :rules="usernameRules" />
        <ErrorMessage name="username" />

        <Field name="password" :rules="passwordRules" />
        <ErrorMessage name="password" />

        <button>Sign Up</button>
      </form>
    </VeeForm>
  </div>
</template>

<script>
import Api from '@/api/Api';
import { Field, Form as VeeForm, ErrorMessage } from 'vee-validate';
import * as yup from 'yup';

export default {
  components: {
    Field,
    VeeForm,
    ErrorMessage,
  },
  data() {
    return {
      form: {
        username: '',
        password: '',
        passwordVerify: '',
      },
      usernameRules: yup.string().required().min(4).label('Username'),
      passwordRules: yup.string().required().min(8).label('Password'),
    };
  },
  mounted() {
    window.localStorage.token = null;
  },
  methods: {
    async onSubmit(values) {
      console.log(values);
      const response = await Api.signup(values);
      if (response.status === 200) {
        const responseJson = await response.json();
        if (responseJson.success) {
          window.localStorage.token = responseJson.token;
          this.$router.push({ name: 'Home' });
        } else {
          this.showToast('Login Failed', responseJson.message, 'danger');
        }
      } else {
        console.error(`Error: ${response}`);
      }
    },
    onReset(event) {
      event.preventDefault();
    },
    showToast(title, message, variant) {
      this.$bvToast.toast(message, {
        title,
        variant,
      });
    },
  },
};
</script>
