<template>
  <q-layout view="lHh lpR fff">

    <q-header elevated height-hint="98">
      <q-toolbar>
        <q-toolbar-title class="wrapper">
          <main-logo />
          <div class="bubble small"></div>
          <div class="bubble small-l"></div>
          <div class="bubble s-medium"></div>
          <q-avatar>
            <img src="https://cdn.quasar.dev/logo-v2/svg/logo-mono-white.svg">
          </q-avatar>
        </q-toolbar-title>
      </q-toolbar>

      <q-tabs align="left">
        <q-route-tab to="/status" label="Status" />
        <q-route-tab to="/logs" label="Logs" />
        <q-route-tab to="/configuration" label="Configuration" />
      </q-tabs>
    </q-header>

    <q-page-container>
      <router-view v-slot="{ Component }">
        <transition
          enter-active-class="animated backInLeft"
        >
          <component :is="Component" />
        </transition>
      </router-view>
    </q-page-container>

    <q-footer class="bg-grey-8">
      <q-toolbar>
        <q-toolbar-title>
          <q-avatar>
            <img src="https://cdn.quasar.dev/logo-v2/svg/logo-mono-white.svg">
          </q-avatar>
          <div>Romcon</div>
        </q-toolbar-title>
      </q-toolbar>
    </q-footer>

  </q-layout>
</template>

<script setup lang="ts">
import MainLogo from 'components/MainLogo.vue';
</script>

<style scoped lang="scss">
body{
  background: #4AC29A;  /* fallback for old browsers */
  background: -webkit-linear-gradient(to right, #BDFFF3, #4AC29A);  /* Chrome 10-25, Safari 5.1-6 */
  background: linear-gradient(to right, #BDFFF3, #4AC29A); /* W3C, IE 10+/ Edge, Firefox 16+, Chrome 26+, Opera 12+, Safari 7+ */
}
.wrapper{
  width: 100%;
  height: 100%;
}
.main-text{
  position: absolute;
  top: 45%;
  left: 35%;
  font-family: 'Lobster', cursive;
  font-size: 5em;
  color: #fff;
}
.bubble{
  position: absolute;
  top: 53%;
  border-radius: 50%;
  border: 1px solid rgba(255, 255, 255, 0.8);
}
.small{
  width: 10px;
  height: 10px;
  left: 50%;
  animation: flow-s 2s linear infinite;
}
.small-l{
  width: 10px;
  height: 10px;
  left: 43%;
  animation: flow-s-l 0.9s linear infinite;
}
.s-medium{
  width: 20px;
  height: 20px;
  left: 43%;
  animation: flow-s 1.3s linear infinite;
}
.medium{
  width: 25px;
  height: 25px;
  left: 45%;
  animation: flow-m 0.8s linear infinite;
}
.large{
  width:5px;
  height: 5px;
  left: 57%;
  animation: flow-l 0.7s linear infinite;
}

@keyframes flow-l {
  0% {
    top: 53%;
    left: 57%;
    opacity:0;
  }
  25%{
    top: 49%;
    left: 57.5%;
    opacity: 1;
  }
  50%{
    left: 58%;
    opacity: 1;
  }
  75%{
    left: 57.5%;
    opacity: 1;
  }
  100% {
    top: 32%;
    left: 57%;
    opacity: 0;
  }
}

@keyframes flow-s {
  0% {
    top: 53%;
    left: 50%;
    opacity:0;
  }
  25%{
    top: 49%;
    left: 51%;
    opacity: 1;
  }
  50%{
    top: 45%;
    left: 52%;
    opacity: 1;
  }
  75%{
    top: 41%;
    left: 51%;
    opacity: 1;
  }
  100% {
    top: 37%;
    left: 50%;
    opacity: 0;
  }
}

@keyframes flow-s-m {
  0% {
    top: 53%;
    left: 43%;;
    opacity:0;
  }
  25%{
    top: 49%;
    left: 42.5%;
    opacity: 1;
  }
  50%{
    top: 45%;
    left: 42%;
    opacity: 1;
  }
  75%{
    top: 41%;
    left: 42.5%;
    opacity: 1;
  }
  100% {
    top: 37%;
    left: 43%;
    opacity: 0;
  }
}

@keyframes flow-m {
  0% {
    top: 53%;
    left: 45%;;
    opacity:0;
  }
  25%{
    top: 48%;
    left: 44.5%;
    opacity: 1;
  }
  50%{
    top: 43%;
    left: 44%;
    opacity: 1;
  }
  75%{
    top: 38%;
    left: 44.5%;
    opacity: 1;
  }
  100% {
    top: 33%;
    left: 45%;
    opacity: 0;
  }
}

@keyframes flow-s-l {
  0% {
    top: 53%;
    left: 43%;;
    opacity:0;
  }
  25%{
    top: 48%;
    left: 43.5%;
    opacity: 1;
  }
  50%{
    top: 43%;
    left: 44%;
    opacity: 1;
  }
  75%{
    top: 38%;
    left: 43.5%;
    opacity: 1;
  }
  100% {
    top: 33%;
    left: 43%;
    opacity: 0;
  }
}
</style>
