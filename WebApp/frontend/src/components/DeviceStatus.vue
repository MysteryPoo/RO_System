<template>
    <div v-if="show">
      <Card style="width: 25rem; margin-bottom: 2em">
        <template #title>
          Current State
        </template>
        <template #content>
          {{ deviceStatus ? currentState : "Offline" }}
          <RemainingTime v-if="currentState === 'FLUSH' || currentState === 'FILL'" :startTime="stateStartTime" :estimatedElapsedSeconds="currentState === 'FLUSH' ? 300 : props.averageFillTime" />
        </template>
      </Card>
    </div>
</template>

<script setup>
import { ref, defineProps, watch } from 'vue';
import { useRouter } from 'vue-router';
import Card from 'primevue/card';
import RemainingTime from '@/components/RemainingTime.vue';

const router = useRouter();
const props = defineProps({
    show: Boolean,
    deviceId: String,
    averageFillTime: Number,
});
const deviceStatus = ref(false);
const currentState = ref("Unknown");
const stateStartTime = ref(null);

const unauthorizedMessage = "Unauthorized";
const deviceIdRequiredMessage = "No device provided";

const getStatus = async (deviceId) => {
  if (deviceId !== null) {
    const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/status`, {
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
};

const getState = async (deviceId) => {
  if (deviceId !== null) {
    const stateRequest = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/currentState`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (stateRequest.status === 200) {
      const response = await stateRequest.json();
      if (response.length > 0) {
        return response[0];
      }
    }
    if (stateRequest.status === 401) {
      router.push('/login');
      throw Error(unauthorizedMessage);
    }
  }
  throw Error(deviceIdRequiredMessage);
};

watch( () => props.deviceId, async (newDeviceId, previousDeviceId) => {
    try {
      const statusRequest = await getStatus(newDeviceId);
      deviceStatus.value = statusRequest.online;
      const stateRequest = await getState(newDeviceId);
      currentState.value = stateRequest.data.state;
      stateStartTime.value = new Date(stateRequest.datetime);
    } catch( err ) {
        console.log(previousDeviceId);
        console.log(err);
    }
});
</script>
