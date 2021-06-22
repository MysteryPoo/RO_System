<template>
  <div>
    <b-form @submit="onSubmit" @reset="onReset">
      <b-form-group
        id="form-group-username"
        label="Username"
        label-for="username"
        description="Username"
      >
        <b-form-input
          id="username"
          v-model="form.username"
          placeholder="Username"
          required
        />
      </b-form-group>
      <b-form-group
        id="form-group-password"
        label="Password"
        label-for="password"
        description="Password"
      >
        <b-form-input
          id="password"
          v-model="form.password"
          placeholder="Password"
          type="password"
          required
        />
      </b-form-group>
      <b-button type="submit" variant="primary">Login</b-button>
    </b-form>
  </div>
</template>

<script>
import Api from '@/api/Api';

export default {
  data() {
    return {
      form: {
        username: '',
        password: '',
      },
    };
  },
  mounted() {
    window.localStorage.token = null;
  },
  methods: {
    async onSubmit(event) {
      event.preventDefault();
      const response = await Api.login(this.form);
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
