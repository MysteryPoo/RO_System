<template>
  <div>
    <div class="text-h6">System is <span :style="`color: ${GetColor()}`">{{ isDeviceEnabled ? "Enabled" : "Disabled" }}</span>
      <q-tooltip anchor="center middle"
        transition-show="rotate"
        transition-hide="rotate"
      >
        Last Changed: {{ lastChecked }}
      </q-tooltip>
      <q-tooltip v-if="isStale" anchor="bottom right"
        transition-show="rotate"
        transition-hide="rotate"
      >
        Haven't heard from device in awhile.
      </q-tooltip>
    </div>
    <device-state :device-id="props.deviceId" />
    <q-inner-loading :showing="isLoading">
      <q-spinner-gears size="50px" color="primary" />
    </q-inner-loading>
  </div>
</template>

<script setup lang="ts">
import { RealtimeChannel } from '@supabase/realtime-js';
import { useDeviceStore } from 'src/stores/device-store';
import { supabase, RoSystemEnabledTypes, RoSystemEnabledSelect, RoSystemEnabledInsert } from 'src/services/supabase.service';
import { ref, Ref, onBeforeUnmount, onMounted, watch, computed } from 'vue';
import { DateTime } from 'luxon';
import DeviceState from './DeviceState.vue';

const props = defineProps<{
  deviceId: string | undefined
}>();

const deviceStore = useDeviceStore();
const rosystemEnabledChannel : Ref<RealtimeChannel | undefined> = ref(undefined);
const enabled : Ref<RoSystemEnabledSelect | RoSystemEnabledInsert | null> = ref(null);
const isLoading = ref(true);
const isDeviceEnabled = computed(() => {
  return enabled.value?.enabled ?? false;
});
const lastChecked = computed( () : string => {
  const datetime = DateTime.fromISO(enabled.value?.datetime ?? '');
  return datetime.toLocaleString(DateTime.DATETIME_SHORT);
});
const isStale = computed( () : boolean => {
  const device = deviceStore.knownDevices.find(d => d.device_id === props.deviceId);
  if (!device) return true;
  const lastHeard = DateTime.fromISO(device.last_heard);
  const threshold = DateTime.now().minus({ minute: 10 });
  return lastHeard <= threshold;
});

onMounted(async () : Promise<void> => {
  await Refresh();
});

onBeforeUnmount(async () : Promise<void> => await UnsubscribeFromRoSystemEnabled());

watch(() => props.deviceId, async () => await Refresh());

async function Refresh(): Promise<void> {
  isLoading.value = true;
  const device = deviceStore.knownDevices.find(d => d.device_id === props.deviceId);
  if (!device) return;
  enabled.value = (await supabase.from<'rosystem_enabled_reports', RoSystemEnabledTypes>('rosystem_enabled_reports')
    .select()
    .eq('device_id', device.id)
    .order('datetime', { ascending: false })
    .limit(1)
    .single()).data as RoSystemEnabledSelect | null;
  await SubscribeToRoSystemEnabled();
  isLoading.value = false;
}

async function SubscribeToRoSystemEnabled(): Promise<void> {
  await UnsubscribeFromRoSystemEnabled();
  const device = deviceStore.knownDevices.find(d => d.device_id === props.deviceId);
  if (device == null) return;
  rosystemEnabledChannel.value = supabase.channel('rosystem-enabled').on(
    'postgres_changes',
    { event: 'INSERT', schema: 'public', table: 'rosystem_enabled_reports', filter: `device_id=eq.${device.id}`},
    payload => {
      enabled.value = payload.new as RoSystemEnabledInsert;
    }
  )
  .subscribe( (status, err) => {
    if (err) {
      console.log(`Error: ${err}`);
    }
    switch(status) {
      case 'CLOSED':
      case 'CHANNEL_ERROR':
      case 'TIMED_OUT': {
          //SubscribeToRoSystemEnabled();
          break;
      }
    }
  });
}

async function UnsubscribeFromRoSystemEnabled(): Promise<void> {
  if (rosystemEnabledChannel.value) await supabase.removeChannel(rosystemEnabledChannel.value);
}

function GetColor() : string {
  return isDeviceEnabled.value ? (isStale.value ? 'yellow' : 'green') : 'red';
}

</script>
