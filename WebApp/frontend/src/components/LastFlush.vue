<template>
    <div>
      <Card v-if="show" style="width: 25rem; margin-bottom: 2em">
        <template #title>
          Last Flush
        </template>
        <template #content>
          {{ lastFlushDisplay }}
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

const lastFlush = ref(null);
const lastFlushDisplay = computed( () => {
    if (lastFlush.value !== null) {
        return lastFlush.value.toLocaleString('en-US', {
            day: 'numeric',
            month: 'short',
            hour: 'numeric',
            minute: '2-digit'
        });
    }
    return 'Unknown';
});

const unauthorizedMessage = "Unauthorized";
const deviceIdRequiredMessage = "No device provided";

const getTimeOfLastFlush = async (deviceId) => {
  if (deviceId !== null) {
    const request = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/flush`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (request.status === 200) {
      const response = await request.json();
      if (response.length > 0) {
        const flushEvent = response[0];
        if (flushEvent.data.success && flushEvent.data.state === 'FLUSH') {
          return flushEvent.datetime;
        } else {
          return null;
        }
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
        const flushEventTime = await getTimeOfLastFlush(newDeviceId);
        if (flushEventTime) {
          lastFlush.value = new Date(flushEventTime);
        } else {
          lastFlush.value = null;
        }
    } catch (err) {
        console.log(err);
    }
});
</script>
