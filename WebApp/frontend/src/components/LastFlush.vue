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
import { useDevicesApi } from '@/services/devices.ts';

const props = defineProps({
    show: Boolean,
    deviceId: String,
});

const api = useDevicesApi();

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

const getTimeOfLastFlush = async (deviceId) => {
  try {
    const response = await api.getStates(deviceId, 0, 1, ['FLUSH']);
    if (response.length > 0) {
      const flushEvent = response[0];
      if (flushEvent.data.success && flushEvent.data.state === 'FLUSH') {
        return flushEvent.datetime;
      }
    }
  } catch (e) {
    console.log(e);
  }
  return null;
};

watch( () => props.deviceId, async (newDeviceId) => {
    try {
        const flushEventTime = await getTimeOfLastFlush(newDeviceId);
        lastFlush.value = flushEventTime ? new Date(flushEventTime) : null;
    } catch (err) {
        console.log(err);
    }
});
</script>
