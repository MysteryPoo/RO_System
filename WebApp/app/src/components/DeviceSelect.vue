<template>
    <div class="q-mt-md" style="width: 250px;">
        <q-select rounded outlined :loading="isLoading" v-model="selectedDevice" :options="store.knownDevices" optionLabel="name" label="Select device..." :stackLabel="false" @update:modelValue="onDeviceSelected">
            <template v-slot:append>
                <q-avatar>
                    <img src="~assets/photon.png">
                </q-avatar>
            </template>
        </q-select>
    </div>
</template>

<script setup lang="ts">
import { ref, onMounted, Ref } from 'vue';
import { useDeviceService, type DeviceService } from '../services/device.service';
import { useDeviceStore, type IDevice } from 'src/stores/device-store';
import { useQuasar } from 'quasar';

const emit = defineEmits<{
    (e: 'device-selected', id: string | undefined): void
}>();
const api : DeviceService = useDeviceService(process.env.API_URL);
const store = useDeviceStore();
const quasar = useQuasar();
const selectedDevice : Ref<IDevice | undefined> = ref(undefined);
const isLoading = ref(true);

onMounted( async () => {
    store.load();
    // Set values based on local cache for responsiveness
    selectedDevice.value = store.knownDevices.find( element => element.id === store.deviceId);
    try {
        // Update from server
        store.knownDevices = await api.getDeviceList();
        selectedDevice.value = store.knownDevices.find( element => element.id === store.deviceId);
        store.deviceId = selectedDevice.value?.id;
        store.save();
        if (selectedDevice.value === undefined) {
            quasar.notify({
                type: 'warning',
                message: 'Previously selected device has been removed.',
                position: 'top',
            })
        }
    } catch (e: unknown) {
        quasar.notify({
            type: 'negative',
            message: 'Could not retrieve list of devices from server.',
            position: 'top',
        })
    } finally {
        isLoading.value = false;
        emit('device-selected', store.deviceId);
    }
});

const onDeviceSelected = (device : IDevice) => {
    store.deviceId = device.id;
    store.save();
    quasar.notify({
        type: 'info',
        message: `Selected device: ${device.name}`,
        position: 'top',
    })
    emit('device-selected', device.id);
};

</script>
