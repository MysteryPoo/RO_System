<template>
    <div>
        <h1 v-if="show">Current State: {{ deviceStatus ? currentState : "Offline" }}</h1>
    </div>
</template>

<script setup>
import { ref, defineProps, watch } from 'vue';
import { useRouter } from 'vue-router';

const router = useRouter();
const props = defineProps({
    show: Boolean,
    deviceId: String,
});
const deviceStatus = ref(false);
const currentState = ref("Unknown");

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
        return response[0].data.state;
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
      currentState.value = stateRequest;
    } catch( err ) {
        console.log(previousDeviceId);
        console.log(err);
    }
});
</script>
