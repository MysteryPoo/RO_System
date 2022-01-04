<template>
  <div class="home">
    <DeviceSelect @deviceSelected="onDeviceSelected"/>
    <DeviceStatus :show="true" :deviceId="deviceId" />
    <LastFlush :show="true" :deviceId="deviceId" />
    <Log :logData="deviceLogs" />
  </div>
</template>

<script lang="ts" setup>
import { ref, watch, onMounted } from 'vue';
import { useRouter } from 'vue-router';
import { useStore } from 'vuex';
import DeviceSelect from '@/components/DeviceSelect.vue';
import DeviceStatus from '@/components/DeviceStatus.vue';
import LastFlush from '@/components/LastFlush.vue';
import Log from '@/components/Log.vue';

const router = useRouter();
const store = useStore();

const unauthorizedMessage = "Unauthorized";
const deviceIdRequiredMessage = "No device provided";

const deviceId = ref('');
const deviceLogs = ref([]);

const onDeviceSelected = (_deviceId) => {
  deviceId.value = _deviceId;
};

const getLogs = async (deviceId) => {
  if (deviceId !== null) {
    const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/logs`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      return response.json();
    }
    if (response.status === 401) {
      return Promise.reject(Error(unauthorizedMessage));
    }
  }
  return Promise.reject(Error(deviceIdRequiredMessage));
}

watch( () => deviceId.value, async () => {
    const logs = await getLogs(deviceId.value);
    deviceLogs.value = logs;
});

onMounted( () => {
  if (store.state.token === undefined) {
    router.replace('/login');
  }
});
</script>
