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
    } catch( e ) {
        if (e instanceof UnauthorizedException && e.code === 401) {
          router.replace('/login');
        }
    }
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
