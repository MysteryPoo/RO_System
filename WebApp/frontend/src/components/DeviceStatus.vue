<template>
    <div v-if="show">
      <Card style="width: 25rem; margin-bottom: 2em; background: linear-gradient(to bottom right, rgb(27, 34, 99), rgb(17, 20, 48));">
        <template #title>
          Current State
        </template>
        <template #content>
          {{ deviceStatus ? currentState : "Offline" }}
          <div v-show="deviceStatus">
            <RemainingTime v-if="currentState === 'FLUSH' || currentState === 'FILL'" :startTime="stateStartTime" :estimatedElapsedSeconds="currentState === 'FLUSH' ? 300 : props.averageFillTime" />
            <p>Firmware Version: <Chip :label="version" /></p>
            <p v-if="volume">~{{ volume }} Gallons</p>
            <p>System is <span :style="enabled ? 'color: green' : 'color: red'">{{ enabled ? "Enabled" : "Disabled" }}</span></p>
            <i v-show="wifiSignal" class="pi pi-wifi" :style=wifiColor>{{ Math.round(wifiSignal) }}%</i>
            <FeatureList :deviceId="props.deviceId" />
          </div>
        </template>
      </Card>
    </div>
</template>

<script setup lang="ts">
import { ref, Ref, onBeforeUnmount, onMounted, watch, computed } from 'vue';
import { useRouter } from 'vue-router';
import Card from 'primevue/card';
import Chip from 'primevue/chip';
import RemainingTime from '@/components/RemainingTime.vue';
import { useDevicesApi, UnauthorizedException } from '@/services/devices';
import FeatureList from './FeatureList.vue';

interface IState {
  datetime: number;
  data: {
    state: string;
    success: boolean;
    requestReason: string;
    failureReason: string;
  };
}

const router = useRouter();
const api = useDevicesApi();
const props = defineProps({
    show: Boolean,
    deviceId: String,
    averageFillTime: Number,
});
const deviceStatus = ref(false);
const currentState = ref("Unknown");
const stateStartTime : Ref<Date | undefined> = ref(undefined);
const version = ref('');
const enabled = ref(true);
const wifiSignal = ref(0);
const wifiQuality = ref(0);
const refreshInterval : Ref<number | undefined> = ref(undefined);
const wifiColor = computed( () => {
  if (wifiQuality.value > 75) {
    return "color: green";
  } else if (wifiQuality.value > 50) {
    return "color: yellow";
  } else {
    return "color: red";
  }
});
const volume : Ref<number | undefined> = ref(undefined);

const initializeRefreshInterval = (deviceId : string | undefined) => {
    if (undefined === deviceId) {
        return;
    }
    if (refreshInterval.value !== undefined) {
      clearInterval(refreshInterval.value);
      refreshInterval.value = undefined;
    }
    refreshInterval.value = setInterval( () => {
      callApi(deviceId);
    }, 10000);
};

const callApi = async (deviceId: string | undefined) => {
  if (deviceId === undefined) {
    return;
  }
  try {
      const statusRequest = await api.getStatus(deviceId);
      deviceStatus.value = statusRequest.online;
      const stateRequest: Array<any> = await api.getStates(deviceId, 0, 1, undefined, true);
      const currentStateData: IState = stateRequest.find( (state) => state.data.success === true );
      currentState.value = currentStateData.data.state;
      stateStartTime.value = new Date(currentStateData.datetime);
      const heartbeat : Array<any> = await api.getHeartbeat(deviceId);
      version.value = heartbeat[0].data.version;
      enabled.value = heartbeat[0].data['ro-system']?.enabled;
      wifiSignal.value = heartbeat[0].data.WiFi?.signal;
      wifiQuality.value = heartbeat[0].data.WiFi?.quality;
      volume.value = convertVoltageToGallons(heartbeat[0].data['float-meter']?.voltage);
    } catch( e ) {
        if (e instanceof UnauthorizedException && e.code === 401) {
          router.replace('/login');
        }
    }
};

const convertVoltageToGallons = (voltage: number | undefined) : number | undefined => {
  if (undefined === voltage) {
    return undefined;
  }
  const table = [
    {Voltage: 1.127, Gallons: 198.334},
    {Voltage: 1.265, Gallons: 195.001},
    {Voltage: 1.384, Gallons: 191.668},
    {Voltage: 1.591, Gallons: 188.335},
    {Voltage: 1.762, Gallons: 185.002},
    {Voltage: 1.904, Gallons: 181.669},
    {Voltage: 1.969, Gallons: 178.336},
    {Voltage: 2.057, Gallons: 175.003},
    {Voltage: 2.123, Gallons: 171.67},
    {Voltage: 2.202, Gallons: 168.337},
    {Voltage: 2.282, Gallons: 165.004},
    {Voltage: 2.347, Gallons: 161.671},
    {Voltage: 2.373, Gallons: 158.338},
    {Voltage: 2.428, Gallons: 155.005},
    {Voltage: 2.466, Gallons: 151.672},
    {Voltage: 2.496, Gallons: 148.339},
    {Voltage: 2.508, Gallons: 145.006},
    {Voltage: 2.518, Gallons: 141.673},
    {Voltage: 2.558, Gallons: 138.34},
    {Voltage: 2.59, Gallons: 135.007},
    {Voltage: 2.598, Gallons: 131.674},
    {Voltage: 2.613, Gallons: 128.341},
    {Voltage: 2.675, Gallons: 125.008},
    {Voltage: 2.686, Gallons: 121.675},
    {Voltage: 2.725, Gallons: 118.342},
    {Voltage: 2.719, Gallons: 115.009},
    {Voltage: 2.733, Gallons: 111.676},
    {Voltage: 2.758, Gallons: 108.343},
    {Voltage: 2.782, Gallons: 105.01},
    {Voltage: 2.798, Gallons: 101.677},
    {Voltage: 2.815, Gallons: 98.344},
    {Voltage: 2.827, Gallons: 95.011},
    {Voltage: 2.84, Gallons: 91.678},
    {Voltage: 2.845, Gallons: 88.345},
    {Voltage: 2.859, Gallons: 85.012},
    {Voltage: 2.864, Gallons: 81.679},
    {Voltage: 2.871, Gallons: 78.346},
    {Voltage: 2.912, Gallons: 75.013}
  ];
  table.sort((a, b) => {
    return a.Voltage - b.Voltage
  });
  table.forEach((tick) => {
    if (voltage >= tick.Voltage) {
      return tick.Gallons;
    }
  });
  return 0;
};

onMounted( () => {
  initializeRefreshInterval(props.deviceId);
});

onBeforeUnmount( () => {
  if (refreshInterval.value === undefined) {
    return;
  }
  clearInterval(refreshInterval.value);
  refreshInterval.value = undefined;
});

watch( () => props.deviceId, (newDeviceId) => {
  callApi(newDeviceId);
  initializeRefreshInterval(newDeviceId);
});

</script>
