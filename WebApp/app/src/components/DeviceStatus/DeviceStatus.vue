<template>
    <div>
      <q-card style="background: linear-gradient(to bottom right, rgb(27, 34, 99), rgb(17, 20, 48));">
        <q-card-section>
          <div class="text-h1">Status</div>
        </q-card-section>
        <q-card-section>
          <div v-if="!isDeviceOnline">
            <div class="text-h3">System cannot be reached.</div>
            <div class="text-h4">Last seen {{ lastOnline }}</div>
          </div>
          <div v-show="isDeviceOnline">
            <firmware-version :device-id="props.deviceId" />
            <wi-fi :device-id="props.deviceId" />
            <ro-system :device-id="props.deviceId" />
            <div class="text-h6" v-if="float !== undefined">Float is <span :style="float ? 'color: red' : 'color: green'">{{ float ? "Full" : "Not Full" }}</span></div>
            <FeatureList :deviceId="props.deviceId" />
          </div>
        </q-card-section>
      </q-card>
    </div>
</template>

<script setup lang="ts">
import { ref, Ref, onMounted, computed } from 'vue';
import { DateTime } from 'luxon';
import { useDeviceStore } from 'src/stores/device-store';
import { DeviceListRow } from 'src/services/supabase.service';
import FeatureList from 'src/components/FeatureList.vue';
import FirmwareVersion from './FirmwareVersion.vue';
import RoSystem from './RoSystem/RoSystem.vue';
import WiFi from './Wi-fi.vue';

const props = defineProps<{
  deviceId: string | undefined,
}>();

const device: Ref<DeviceListRow | undefined> = ref(undefined);
const isDeviceOnline = computed( () : boolean => {
  return device.value?.online ?? false;
});
const lastOnline = computed( () : string => {
  return DateTime.fromISO(device.value?.last_heard ?? '').toLocaleString(DateTime.DATETIME_SHORT);
});
const float : Ref<boolean | undefined> = ref(undefined);

const deviceStore = useDeviceStore();

onMounted( () => {
  device.value = deviceStore.knownDevices.find(d => d.device_id === props.deviceId);
  deviceStore.$subscribe((_mutation, state) => {
    device.value = state.knownDevices.find(d => d.device_id === props.deviceId);
  });
});

</script>
