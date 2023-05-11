<template>
  <div>
    <i class="text-h6" v-show="wifiSignal" :style=wifiColor><q-icon :name="matWifi" />{{ Math.round(wifiSignal?.signal ?? 0) }}%
      <q-tooltip anchor="center middle"
        transition-show="rotate"
        transition-hide="rotate"
      >
        Quality: {{ Math.round(wifiQuality?.quality ?? 0) }}
      </q-tooltip>
      <q-tooltip anchor="bottom right"
        transition-show="rotate"
        transition-hide="rotate"
      >
        Last Changed: {{ lastChecked }}
      </q-tooltip>
    </i>
    <q-inner-loading :showing="isLoading">
      <q-spinner-gears size="50px" color="primary" />
    </q-inner-loading>
  </div>
</template>

<script setup lang="ts">
import { useDeviceStore } from 'src/stores/device-store';
import { supabase, WifiSignalTypes, WifiSignalSelect, WifiSignalInsert, WifiQualityTypes, WifiQualitySelect, WifiQualityInsert } from 'src/services/supabase.service';
import { ref, Ref, onBeforeUnmount, onMounted, watch, computed } from 'vue';
import { matWifi } from '@quasar/extras/material-icons';
import { DateTime } from 'luxon';
import { useSubscribe } from 'src/composables/subscribe';

const props = defineProps<{
  deviceId: string | undefined
}>();

const deviceStore = useDeviceStore();
const signalSubscription = useSubscribe();
const qualitySubscription = useSubscribe();
const wifiSignal : Ref<WifiSignalSelect | WifiSignalInsert | null> = ref(null);
const wifiQuality : Ref<WifiQualitySelect | WifiQualityInsert | null> = ref(null);
const isLoading = ref(true);
const wifiColor = computed( () => {
  if (wifiQuality.value?.quality ?? 0 > 75) {
    return 'color: green';
  } else if (wifiQuality.value?.quality ?? 0 > 50) {
    return 'color: yellow';
  } else {
    return 'color: red';
  }
});
const lastChecked = computed( () : string => {
  const datetime = DateTime.fromISO(wifiSignal.value?.datetime ?? '');
  return datetime.toLocaleString(DateTime.DATETIME_SHORT);
});

onMounted(async () : Promise<void> => {
  await Refresh();
});

onBeforeUnmount(async () : Promise<void> => await UnsubscribeFromWifi());

watch(() => props.deviceId, async () => await Refresh());

async function Refresh(): Promise<void> {
  isLoading.value = true;
  const device = deviceStore.knownDevices.find(d => d.device_id === props.deviceId);
  if (!device) return;
  wifiSignal.value = (await supabase.from<'wifi_signal_reports', WifiSignalTypes>('wifi_signal_reports')
    .select()
    .eq('device_id', device.id)
    .order('datetime', { ascending: false })
    .limit(1)
    .single()).data as WifiSignalSelect | null;
  wifiQuality.value = (await supabase.from<'wifi_quality_reports', WifiQualityTypes>('wifi_quality_reports')
    .select()
    .eq('device_id', device.id)
    .order('datetime', { ascending: false })
    .limit(1)
    .single()).data as WifiQualitySelect | null;
  await SubscribeToWifi();
  isLoading.value = false;
}

function SubscribeToWifiSignal(): void {
  if (!props.deviceId) return;
  UnsubscribeFromWifiSignal();
  signalSubscription.Subscribe(props.deviceId, 'wifi_signal', 'wifi_signal_reports', payload => {
    wifiSignal.value = payload.new as WifiSignalInsert;
  });
}

async function UnsubscribeFromWifiSignal(): Promise<void> {
  await signalSubscription.Unsubscribe();
}

async function SubscribeToWifiQuality(): Promise<void> {
  if (!props.deviceId) return;
  UnsubscribeFromWifiQuality();
  qualitySubscription.Subscribe(props.deviceId, 'wifi_quality', 'wifi_quality_reports', payload => {
    wifiQuality.value = payload.new as WifiQualityInsert;
  });
}

async function UnsubscribeFromWifiQuality(): Promise<void> {
  await qualitySubscription.Unsubscribe();
}

async function SubscribeToWifi(): Promise<void> {
  await SubscribeToWifiSignal();
  await SubscribeToWifiQuality();
}

async function UnsubscribeFromWifi(): Promise<void> {
  await UnsubscribeFromWifiSignal();
  await UnsubscribeFromWifiQuality();
}

</script>
