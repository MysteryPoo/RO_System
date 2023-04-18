<template>
    <div v-if="show">
      <q-card style="background: linear-gradient(to bottom right, rgb(27, 34, 99), rgb(17, 20, 48));">
        <q-card-section>
          <div class="text-h1">Status</div>
        </q-card-section>
        <q-card-section>
          <div class="text-h3">Current State: {{ deviceStatus ? currentState : "Offline" }}</div>
          <div v-show="deviceStatus">
            <RemainingTime v-if="currentState === 'FLUSH' || currentState === 'FILL'" :startTime="stateStartTime" :estimatedElapsedSeconds="currentState === 'FLUSH' ? 300 : props.averageFillTime" />
            <div class="text-h6">Firmware Version: <q-chip color="primary">{{ version }}</q-chip></div>
            <div class="text-h6">System is <span :style="enabled ? 'color: green' : 'color: red'">{{ enabled ? "Enabled" : "Disabled" }}</span></div>
            <i v-show="wifiSignal" :style=wifiColor><q-icon :name="matWifi" />{{ Math.round(wifiSignal) }}%</i>
            <FeatureList :deviceId="props.deviceId" />
          </div>
        </q-card-section>
        <q-inner-loading :showing="isLoadingFirstTime">
          <q-spinner-gears size="50px" color="primary" />
        </q-inner-loading>
      </q-card>
    </div>
</template>

<script setup lang="ts">
import { ref, Ref, onBeforeUnmount, onMounted, watch, computed } from 'vue';
import { Heartbeat, getHeartbeat, getStateRequests, supabase } from 'src/services/supabase.service';
import { matWifi } from '@quasar/extras/material-icons';
import FeatureList from 'src/components/FeatureList.vue';
import RemainingTime from './RemainingTime.vue';
import { RealtimeChannel } from '@supabase/realtime-js';

// TODO : Make deviceId prop into the actual device object, including its ID etc to reduce DB calls

const props = defineProps({
    show: Boolean,
    deviceId: String,
    averageFillTime: Number,
});
const isLoadingFirstTime = ref(true);
const deviceStatus = ref(false);
const currentState = ref('Unknown');
const stateStartTime : Ref<Date | undefined> = ref(undefined);
const version = ref('');
const enabled = ref(true);
const wifiSignal = ref(0);
const wifiQuality = ref(0);
const wifiColor = computed( () => {
  if (wifiQuality.value > 75) {
    return 'color: green';
  } else if (wifiQuality.value > 50) {
    return 'color: yellow';
  } else {
    return 'color: red';
  }
});
const heartbeatChannel : Ref<RealtimeChannel | undefined> = ref(undefined);

async function getStatus() {
  if (props.deviceId == undefined) return;
  const heartbeat = await getHeartbeat(props.deviceId);
  if (heartbeat) updateHeartbeatInfo(heartbeat);
  const state = (await getStateRequests(props.deviceId))?.[0];
  currentState.value = state?.state ?? 'Unknown';
  stateStartTime.value = new Date(state?.datetime ?? 0) ?? undefined;
  isLoadingFirstTime.value = false;
}

function updateHeartbeatInfo(heartbeat: Heartbeat) {
  deviceStatus.value = heartbeat.device.online;
  version.value = heartbeat.base.version;
  enabled.value = heartbeat.rosystem?.enabled ?? false;
  wifiSignal.value = heartbeat.wifi?.signal ?? 0;
  wifiQuality.value = heartbeat.wifi?.quality ?? 0;
}

onMounted( () => {
  SubscribeToHeartbeats();
});

onBeforeUnmount( () => {
  UnSubscribeToHeartbeats();
});

watch( () => props.deviceId, () => {
  getStatus();
  SubscribeToHeartbeats();
});

async function SubscribeToHeartbeats() {
  UnSubscribeToHeartbeats();
  if (!props.deviceId) return;
  const device = await (await supabase.from('device_list').select().eq('device_id', props.deviceId).limit(1).single()).data;
  if (!device) return;
  heartbeatChannel.value = supabase.channel('schema-db-changes').on(
    'postgres_changes',
    { event: 'INSERT', schema: 'public', table: 'heartbeat', filter: `device_id=eq.${device.id}`},
    async payload => {
      console.log(payload);
      //updateHeartbeatInfo(payload.new as Heartbeat);
      // TODO : Utilize the payload
      if (props.deviceId) {
        isLoadingFirstTime.value = true;
        const heartbeat = await getHeartbeat(props.deviceId);
        if (heartbeat) updateHeartbeatInfo(heartbeat);
        isLoadingFirstTime.value = false;
      }
    })
    .subscribe();
}

function UnSubscribeToHeartbeats() {
  if (heartbeatChannel.value) supabase.removeChannel(heartbeatChannel.value);
}

</script>
