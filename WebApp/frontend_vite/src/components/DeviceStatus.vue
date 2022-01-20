<template>
    <div v-if="show">
      <Card style="width: 25rem; margin-bottom: 2em; background: linear-gradient(to bottom right, rgb(27, 34, 99), rgb(17, 20, 48));">
        <template #title>
          Current State
        </template>
        <template #content>
          {{ deviceStatus ? currentState : "Offline" }}
          <RemainingTime v-if="currentState === 'FLUSH' || currentState === 'FILL'" :startTime="stateStartTime" :estimatedElapsedSeconds="currentState === 'FLUSH' ? 300 : props.averageFillTime" />
          <p>Firmware Version: {{ version }}</p>
          <p>System is {{ enabled ? "Enabled" : "Disabled" }}</p>
        </template>
      </Card>
    </div>
</template>

<script setup lang="ts">
import { ref, Ref, defineProps, watch } from 'vue';
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

watch( () => props.deviceId, async (newDeviceId) => {
    try {
      const statusRequest = await api.getStatus(newDeviceId);
      deviceStatus.value = statusRequest.online;
      const stateRequest = await api.getStates(newDeviceId, 0, 1);
      currentState.value = stateRequest[0].data.state;
      stateStartTime.value = new Date(stateRequest[0].datetime);
      const ticks = await api.getTicks(newDeviceId);
      version.value = ticks[0].data.version;
      enabled.value = ticks[0].data.enabled;
    } catch( e ) {
        if (e instanceof UnauthorizedException && e.code === 401) {
          router.replace('/login');
        }
    }
});
</script>
