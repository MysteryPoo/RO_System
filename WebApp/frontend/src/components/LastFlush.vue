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

watch( () => props.deviceId, async (newDeviceId) => {
    try {
        const flushEventTime = await api.getTimeOfLastFlush(newDeviceId);
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
