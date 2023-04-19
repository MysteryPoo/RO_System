<template>
    <div class="q-mt-md" style="width: 250px;">
        <q-select rounded outlined :loading="isLoading" v-model="selectedDevice" :options="store.knownDevices" optionLabel="device_name" label="Select device..." :stackLabel="false" @update:modelValue="onDeviceSelected">
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
import { useDeviceStore } from 'src/stores/device-store';
import { useQuasar } from 'quasar';
import { DeviceListRow, getDevices, supabase } from 'src/services/supabase.service';
import { RealtimeChannel } from '@supabase/realtime-js';

const emit = defineEmits<{
    (e: 'device-selected', id: string | undefined): void
}>();
const store = useDeviceStore();
const quasar = useQuasar();
const selectedDevice : Ref<DeviceListRow | undefined> = ref(undefined);
const isLoading = ref(true);
const deviceListChannel : Ref<RealtimeChannel | undefined> = ref(undefined);

onMounted( async () => {
    store.load();
    SubscribeToDeviceList();
    // Set values based on local cache for responsiveness
    selectedDevice.value = store.knownDevices.find( element => element.device_id === store.deviceId);
    try {
        // Update from server
        const deviceList = await getDevices();
        updateDeviceStore(deviceList.data);
        selectedDevice.value = store.knownDevices.find( element => element.device_id === store.deviceId);
        store.deviceId = selectedDevice.value?.device_id;
        store.save();
        if (selectedDevice.value === undefined) {
            quasar.notify({
                type: 'warning',
                message: 'Previously selected device has been removed.',
                position: 'top',
            });
        }
    } catch (e: unknown) {
        quasar.notify({
            type: 'negative',
            message: 'Could not retrieve list of devices from server.',
            position: 'top',
        });
    } finally {
        isLoading.value = false;
        emit('device-selected', store.deviceId);
    }
});

function SubscribeToDeviceList() {
    UnSubscribeToDeviceList();
    deviceListChannel.value = supabase.channel('schema-db-changes').on(
        'postgres_changes',
        { event: '*', schema: 'public', table: 'device_list'},
        payload => {
            console.log(payload);
            switch(payload.eventType) {
                case 'UPDATE': {
                    const update : DeviceListRow = payload.new as DeviceListRow;
                    const device = store.knownDevices.find((d => d.id === update.id));
                    if (device) {
                        device.connected = update.connected;
                        device.device_id = update.device_id;
                        device.device_name = update.device_name;
                        device.online = update.online;
                    }
                    break;
                }
                case 'INSERT':
                    store.knownDevices.push(payload.new as DeviceListRow);
                    break;
                case 'DELETE': {
                    const toDelete : DeviceListRow = payload.new as DeviceListRow;
                    const index = store.knownDevices.findIndex((d => d.id === toDelete.id));
                    if (index >= 0) store.knownDevices.splice(index, 1);
                    break;
                }
            }
            store.save();
        }
    )
    .subscribe();
}

function UnSubscribeToDeviceList() {
    if (deviceListChannel.value) supabase.removeChannel(deviceListChannel.value);
}

function onDeviceSelected(device : DeviceListRow) {
    store.deviceId = device.device_id;
    store.save();
    quasar.notify({
        type: 'info',
        message: `Selected device: ${device.device_name}`,
        position: 'top',
    })
    emit('device-selected', device.device_id);
};

function updateDeviceStore(response: Array<DeviceListRow> | null) {
    if (!response) return;
    store.knownDevices = response;
}

</script>
