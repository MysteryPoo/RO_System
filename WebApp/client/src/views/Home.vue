<template>
  <div class="home">
    <img alt="Vue logo" src="../assets/logo.png">
    <log/>
  </div>
</template>

<script>
// @ is an alias to /src
import log from '@/components/Log.vue';

export default {
  name: 'Home',
  components: {
    log,
  },
};
</script>
