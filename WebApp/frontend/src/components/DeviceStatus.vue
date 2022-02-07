<template>
    <div v-if="show">
      <Card style="width: 25rem; margin-bottom: 2em; background: linear-gradient(to bottom right, rgb(27, 34, 99), rgb(17, 20, 48));">
        <template #title>
          Current State
        </template>
        <template #content>
          {{ deviceStatus ? currentState : "Offline" }}
          <div v-show="deviceStatus">
            <RemainingTime v-if="currentState === 'FLUSH' || currentState === 'FILL'" :startTime="stateStartTime" :estimatedElapsedSeconds="currentState === 'FLUSH' ? 300 : props.averageFillTime" />
            <p>Firmware Version: {{ version }}</p>
            <p>System is {{ enabled ? "Enabled" : "Disabled" }}</p>
          </div>
        </template>
      </Card>
    </div>
</template>

<script setup lang="ts">
import { ref, Ref, onBeforeUnmount, onMounted, watch } from 'vue';
import { useRouter } from 'vue-router';
import Card from 'primevue/card';
import RemainingTime from '@/components/RemainingTime.vue';
import { useDevicesApi, UnauthorizedException } from '@/services/devices';

const router = useRouter();
const api = useDevicesApi();
const props = defineProps({
    show: Boolean,
    deviceId: String,
    averageFillTime: Number,
});
const deviceStatus = ref(false);
const currentState = ref("Unknown");
const stateStartTime : Ref<Date | undefined> = ref(undefined);
const version = ref('');
const enabled = ref(true);
const refreshInterval : Ref<number | undefined> = ref(undefined);

const initializeRefreshInterval = (deviceId : string | undefined) => {
    if (undefined === deviceId) {
        return;
    }
    if (refreshInterval.value !== undefined) {
      clearInterval(refreshInterval.value);
      refreshInterval.value = undefined;
    }
    refreshInterval.value = setInterval( () => {
      callApi(deviceId);
    }, 10000);
};

const callApi = async (deviceId: string | undefined) => {
  if (deviceId === undefined) {
    return;
  }
  try {
      const statusRequest = await api.getStatus(deviceId);
      deviceStatus.value = statusRequest.online;
      const stateRequest = await api.getStates(deviceId, 0, 1);
      currentState.value = stateRequest[0].data.state;
      stateStartTime.value = new Date(stateRequest[0].datetime);
      const ticks = await api.getTicks(deviceId);
      version.value = ticks[0].data.version;
      enabled.value = ticks[0].data.enabled;
    } catch( e ) {
        if (e instanceof UnauthorizedException && e.code === 401) {
          router.replace('/login');
        }
    }
};

onMounted( () => {
  initializeRefreshInterval(props.deviceId);
});

onBeforeUnmount( () => {
  if (refreshInterval.value === undefined) {
    return;
  }
  clearInterval(refreshInterval.value);
  refreshInterval.value = undefined;
});

watch( () => props.deviceId, (newDeviceId) => {
  callApi(newDeviceId);
  initializeRefreshInterval(newDeviceId);
});

</script>
