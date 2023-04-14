<template>
    <div v-if="show">
      <q-card style="background: linear-gradient(to bottom right, rgb(27, 34, 99), rgb(17, 20, 48));">
        <q-card-section>
          <div class="text-h1">Current Status</div>
        </q-card-section>
        <q-card-section>
          <div class="text-h3">{{ deviceStatus ? currentState : "Offline" }}</div>
          <div v-show="deviceStatus">
            <div class="text-h6">Firmware Version: <q-chip color="primary">{{ version }}</q-chip></div>
            <p>System is <span :style="enabled ? 'color: green' : 'color: red'">{{ enabled ? "Enabled" : "Disabled" }}</span></p>
            <i v-show="wifiSignal" :style=wifiColor><q-icon :name="matWifi" />{{ Math.round(wifiSignal) }}%</i>
          </div>
        </q-card-section>
      </q-card>
      <!--<Card style="width: 25rem; margin-bottom: 2em; background: linear-gradient(to bottom right, rgb(27, 34, 99), rgb(17, 20, 48));">
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
      </Card>-->
    </div>
</template>

<script setup lang="ts">
import { ref, Ref, onBeforeUnmount, onMounted, watch, computed } from 'vue';
import { useRouter } from 'vue-router';
import { useDeviceService, UnauthorizedException } from '../services/device.service';
import { getHeartbeat } from 'src/services/supabase.service';
import { matWifi } from '@quasar/extras/material-icons';

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
const api = useDeviceService(process.env.API_URL);
const props = defineProps({
    show: Boolean,
    deviceId: String,
    averageFillTime: Number,
});
const deviceStatus = ref(false);
const currentState = ref('Unknown');
const stateStartTime : Ref<Date | undefined> = ref(undefined);
const version = ref('');
const enabled = ref(true);
const wifiSignal = ref(0);
const wifiQuality = ref(0);
const refreshInterval : Ref<number | NodeJS.Timeout | undefined> = ref(undefined);
const wifiColor = computed( () => {
  if (wifiQuality.value > 75) {
    return 'color: green';
  } else if (wifiQuality.value > 50) {
    return 'color: yellow';
  } else {
    return 'color: red';
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
      getStatus();
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
          //router.replace('/login');
        }
    }
};

async function getStatus() {
  if (props.deviceId == undefined) return;
  const heartbeat = await getHeartbeat(props.deviceId);
  if (heartbeat == undefined) return;
  deviceStatus.value = heartbeat.device.online;
  version.value = heartbeat.base.version;
  enabled.value = heartbeat.rosystem?.enabled ?? false;
  wifiSignal.value = heartbeat.wifi?.signal ?? 0;
  wifiQuality.value = heartbeat.wifi?.quality ?? 0;
}

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
  getStatus();
  initializeRefreshInterval(newDeviceId);
});

</script>
