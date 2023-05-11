<template>
  <div>
    <div class="text-h3">Current State: {{ currentState?.state ?? "Unknown" }}</div>
    <div>
      <RemainingTime v-if="currentState?.state === 'FLUSH' || currentState?.state === 'FILL'" :startTime="new Date(currentState.datetime)" :estimatedElapsedSeconds="currentState.state === 'FLUSH' ? 300 : 99" />
    </div>
    <q-inner-loading :showing="isLoading">
      <q-spinner-gears size="50px" color="primary" />
    </q-inner-loading>
  </div>
</template>

<script setup lang="ts">
import { RealtimeChannel } from '@supabase/realtime-js';
import { useDeviceStore } from 'src/stores/device-store';
import { supabase, StateRequestRowTypes, StateRequestSelect, StateRequestInsert, DeviceListRow } from 'src/services/supabase.service';
import { ref, Ref, onBeforeUnmount, onMounted, watch } from 'vue';
import RemainingTime from './RemainingTime.vue';

const props = defineProps<{
  deviceId: string | undefined
}>();

const deviceStore = useDeviceStore();
const device : Ref<DeviceListRow | undefined> = ref(undefined);
const stateRequestChannel : Ref<RealtimeChannel | undefined> = ref(undefined);
const currentState : Ref<StateRequestSelect | StateRequestInsert | null> = ref(null);
const isLoading = ref(true);

onMounted(async () : Promise<void> => {
  await Refresh();
});

onBeforeUnmount(async () : Promise<void> => await UnsubscribeFromStateRequest());

watch(() => props.deviceId, async () => await Refresh());

async function Refresh(): Promise<void> {
  isLoading.value = true;
  device.value = deviceStore.knownDevices.find(d => d.device_id === props.deviceId);
  if (!device.value) return;
  currentState.value = (await supabase.from<'state_request', StateRequestRowTypes>('state_request').select().eq('device_id', device.value.id).order('datetime', { ascending: false }).limit(1).single()).data as StateRequestSelect;
  if (currentState.value) {
    
  }
  deviceStore.$subscribe((_mutation, state) => {
    device.value = state.knownDevices.find(d => d.device_id === props.deviceId);
  });
  await SubscribeToStateRequest();
  isLoading.value = false;
}

async function SubscribeToStateRequest(): Promise<void> {
  await UnsubscribeFromStateRequest();
  const device = deviceStore.knownDevices.find(d => d.device_id === props.deviceId);
  if (device == null) return;
  stateRequestChannel.value = supabase.channel('state_request_insert').on(
    'postgres_changes',
    { event: 'INSERT', schema: 'public', table: 'state_request', filter: `device_id=eq.${device.id}`},
    async payload => {
      currentState.value = payload.new as StateRequestInsert;
    }
  )
  .subscribe((status, err) => {
    if (err) {
      console.log(`Error: ${err}`);
    }
    switch(status) {
      case 'CLOSED':
      case 'CHANNEL_ERROR':
      case 'TIMED_OUT': {
          //SubscribeToStateRequest();
          break;
      }
    }
  });
}

async function UnsubscribeFromStateRequest(): Promise<void> {
  if (stateRequestChannel.value) await supabase.removeChannel(stateRequestChannel.value);
}
</script>
