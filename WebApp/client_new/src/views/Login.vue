<template>
  <div>
    <div class="p-d-flex p-flex-column p-jc-center">
      <div class="p-field">
        <label for="username">Username</label>
        <InputText id="username" type="text" v-model="form.username" />
      </div>
      <div class="p-field">
        <label for="password">Password</label>
        <Password id="password" v-model="form.password" />
      </div>
      
      <Button label="Login"/>
    </div>
  </div>
</template>

<script>
import Api from '@/api/Api';
import * as yup from 'yup';
import Button from 'primevue/button';
import InputText from 'primevue/inputtext';
import Password from 'primevue/password';

export default {
  components: {
    Button,
    InputText,
    Password,
  },
  data() {
    return {
      form: {
        username: '',
        password: '',
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
      const response = await Api.login(values);
      if (response.status === 200) {
        const responseJson = await response.json();
        if (responseJson.success) {
          window.localStorage.token = responseJson.token;
          this.$router.push({ name: 'Home' });
        } else {
          this.showToast('Login Failed', responseJson.message, 'danger');
        }
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

<style scoped>
.box {
  background-color: var(--surface-e);
  text-align: center;
  padding-top: 1rem;
  padding-bottom: 1rem;
  border-radius: 4px;
  box-shadow: 0 2px 1px -1px rgba(0,0,0,.2), 0 1px 1px 0 rgba(0,0,0,.14), 0 1px 3px 0 rgba(0,0,0,.12);
}
</style>
