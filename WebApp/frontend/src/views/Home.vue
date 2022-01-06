<template>
  <div class="home">
    <DeviceSelect @deviceSelected="onDeviceSelected"/>
    <DeviceStatus :show="deviceId !== ''" :deviceId="deviceId" />
    <LastFlush :show="deviceId !== ''" :deviceId="deviceId" />
    <LastRestart :show="deviceId !== ''" :deviceId="deviceId" />
    <Log :show="deviceId !== ''" :deviceId="deviceId" />
    <StateList :show="deviceId !== ''" :deviceId="deviceId" />
  </div>
</template>

<script lang="ts" setup>
import { ref, onMounted } from 'vue';
import { useRouter } from 'vue-router';
import { useStore } from 'vuex';
import DeviceSelect from '@/components/DeviceSelect.vue';
import DeviceStatus from '@/components/DeviceStatus.vue';
import LastFlush from '@/components/LastFlush.vue';
import LastRestart from '@/components/LastRestart.vue';
import Log from '@/components/Log.vue';
import StateList from '@/components/StateList.vue';

const router = useRouter();
const store = useStore();

const deviceId = ref('');

const onDeviceSelected = (_deviceId) => {
  deviceId.value = _deviceId;
};

onMounted( () => {
  if (store.state.token === undefined) {
    router.replace('/login');
  }
});
</script>
