<template>
    <div>
        <h1 v-if="show">Last Restart: {{ lastRestartDisplay }}</h1>
        <h2 v-if="show">Reason: {{ lastRestartReason }}</h2>
    </div>
</template>

<script setup>
import { ref, defineProps, watch, computed } from 'vue';

const props = defineProps({
    show: Boolean,
    deviceId: String,
});

const lastRestart = ref(null);
const lastRestartDisplay = computed( () => {
    if (lastRestart.value !== null) {
        return lastRestart.value.toLocaleString('en-US', {
            day: 'numeric',
            month: 'short',
            hour: 'numeric',
            minute: '2-digit'
        });
    }
    return 'Unknown';
});
const lastRestartReason = ref('');

const unauthorizedMessage = "Unauthorized";
const deviceIdRequiredMessage = "No device provided";

const getTimeOfLastRestart = async (deviceId) => {
  if (deviceId !== null) {
    const request = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/restart`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (request.status === 200) {
      const response = await request.json();
      if (response.length > 0) {
        const restartEvent = response[0];
        return restartEvent;
      }
    }
    if (request.status === 401) {
      throw Error(unauthorizedMessage);
    }
  } else {
    throw Error(deviceIdRequiredMessage);
  }
};

watch( () => props.deviceId, async (newDeviceId) => {
  try {
    const restartEvent = await getTimeOfLastRestart(newDeviceId);
    lastRestart.value = new Date(restartEvent.datetime);
    lastRestartReason.value = restartEvent.data.reason;
  } catch (err) {
    console.log(err);
  }
});
</script>
