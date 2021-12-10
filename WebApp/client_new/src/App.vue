<template>
  <div>
    <img alt="Vue logo" src="@/assets/logo.png">
    <div id="nav">
      <router-link to="/">Home</router-link> |
      <router-link to="/about">About</router-link> |
      <!--<router-link to="/config">Configuration</router-link> |-->
      <router-link to="/login">Login</router-link> |
      <router-link to="/signup">Signup</router-link>
    </div>
    <router-view/>
  </div>
</template>

<script>
export default {
  name: 'App',
  components: {
    
  }
}
</script>

<style>
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  margin-top: 60px;
}
</style>
