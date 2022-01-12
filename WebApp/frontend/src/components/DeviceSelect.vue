<template>
    <div class="p-d-flex p-ai-center">
        <Dropdown :loading="deviceList && deviceList.length === 0" v-model="selectedDevice" :options="deviceList" @change="onDeviceSelected" optionLabel="name" placeholder="Select device..."/>
    </div>
</template>

<script setup>
import { ref, defineEmits, onMounted } from 'vue';
import Dropdown from 'primevue/dropdown';
import { useDevicesApi } from '@/services/devices.ts';

const emit = defineEmits(['deviceSelected']);
const api = useDevicesApi();
const deviceList = ref([]);
const deviceId = ref(null);
const selectedDevice = ref(null);

const onDeviceSelected = (event) => {
    window.localStorage.deviceId = event.value.id;
    emit('deviceSelected', event.value.id);
};

onMounted( async () => {
    deviceId.value = window.localStorage.deviceId;
    const deviceListRequest = await api.getDeviceList();
    deviceList.value = deviceListRequest;
    if(deviceId.value) {
        selectedDevice.value = deviceList.value.find( (element) => element.id === deviceId.value);
        emit('deviceSelected', deviceId.value);
    }
});
</script>
