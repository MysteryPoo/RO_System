<template>
    <div>
      <Card v-if="show" style="width: 25rem; margin-bottom: 2em; background: linear-gradient(to bottom right, rgb(27, 34, 99), rgb(17, 20, 48))">
        <template #title>
          Last Restart
        </template>
        <template #subtitle>
          Reason: {{ lastRestartReason }}
        </template>
        <template #content>
          {{ lastRestartDisplay }}
        </template>
      </Card>
    </div>
</template>

<script setup lang="ts">
import { ref, Ref, defineProps, watch, computed } from 'vue';
import Card from 'primevue/card';
import { useDevicesApi } from '@/services/devices';

const props = defineProps({
    show: Boolean,
    deviceId: String,
});

const api = useDevicesApi();
const lastRestart : Ref<Date | null> = ref(null);
const lastRestartDisplay = computed( () => {
    if (lastRestart.value === null) {
      return 'Unknown';
    }
    return lastRestart.value.toLocaleString('en-US', {
        day: 'numeric',
        month: 'short',
        hour: 'numeric',
        minute: '2-digit'
    });
});
const lastRestartReason = ref('');

const getTimeOfLastRestart = async (deviceId : string | undefined) => {
  const restarts = await api.getRestarts(deviceId);
  if (restarts.length > 0) {
    const restartEvent = restarts[0];
    return restartEvent;
  } else {
    return null;
  }
};

watch( () => props.deviceId, async (newDeviceId) => {
  try {
    const restartEvent = await getTimeOfLastRestart(newDeviceId);
    if (restartEvent) {
      lastRestart.value = new Date(restartEvent.datetime);
      lastRestartReason.value = restartEvent.data.reason;
    } else {
      lastRestart.value = null;
      lastRestartReason.value = '';
    }
  } catch (err) {
    console.log(err);
  }
});
</script>
