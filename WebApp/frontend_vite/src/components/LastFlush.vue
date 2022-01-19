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

<script setup lang="ts">
import { ref, Ref, defineProps, watch, computed } from 'vue';
import Card from 'primevue/card';
import { useDevicesApi } from '@/services/devices';

const props = defineProps({
    show: Boolean,
    deviceId: String,
});

const api = useDevicesApi();

const lastFlush : Ref<Date | null> = ref(null);

const lastFlushDisplay = computed( () => {
    if (lastFlush.value === null) {
      return 'Unknown';
    }
    return lastFlush.value.toLocaleString('en-US', {
        day: 'numeric',
        month: 'short',
        hour: 'numeric',
        minute: '2-digit'
    });
});

const getTimeOfLastFlush = async (deviceId : string | undefined) => {
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
