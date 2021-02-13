<template>
  <div class="home">
    <img alt="Vue logo" src="../assets/logo.png">
    <!--<HelloWorld msg="Welcome to Your Vue.js App"/> -->
    <log/>
  </div>
</template>

<script>
// @ is an alias to /src
// import HelloWorld from '@/components/HelloWorld.vue';
import log from '@/components/log.vue';

export default {
  name: 'Home',
  components: {
    // HelloWorld,
    log,
  },
};
</script>
