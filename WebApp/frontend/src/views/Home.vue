<template>
  <div class="flex flex-column">
    <DeviceSelect @deviceSelected="onDeviceSelected" class="flex align-items-center justify-content-center m-2 px-5 py-3 border-round" />
    <div class="card">
      <div class="flex flex-row flex-wrap justify-content-center">
        <DeviceStatus :show="deviceId !== ''" :deviceId="deviceId" :averageFillTime="averageFillTime" class="flex align-items-center justify-content-center w-16rem m-2" />
        <LastFlush :show="deviceId !== ''" :deviceId="deviceId" class="flex align-items-center justify-content-center w-16rem m-2" />
        <LastRestart :show="deviceId !== ''" :deviceId="deviceId" class="flex align-items-center justify-content-center w-16rem m-2" />
      </div>
    </div>
    <Log class="flex-auto flex align-items-center justify-content-center" :show="deviceId !== ''" :deviceId="deviceId" />
    <StateList class="flex-auto flex align-items-center justify-content-center" :show="deviceId !== ''" :deviceId="deviceId" @averageFillTime="onAverageFillTime" />
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
const averageFillTime = ref(0);

const onDeviceSelected = (_deviceId) => {
  deviceId.value = _deviceId;
};

onMounted( () => {
  if (store.state.token === undefined) {
    router.replace('/login');
  }
});

const onAverageFillTime = (data) => {
  averageFillTime.value = data * 60;
}
</script>
