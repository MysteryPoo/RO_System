<template>
    <div>
      <Card v-if="show" style="width: 25rem; margin-bottom: 2em">
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

<script setup>
import { ref, defineProps, watch, computed } from 'vue';
import Card from 'primevue/card';

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
      } else {
        return null;
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
