<template>
  <div class="text-h6" v-if="float">Float reports <span :style="`color:${color}`">{{ status }}</span>
    <q-tooltip anchor="bottom right"
      transition-show="rotate"
      transition-hide="rotate"
    >
      Last Changed: {{ lastChecked }}
    </q-tooltip>
  </div>
</template>

<script setup lang="ts">
import { useDeviceStore } from 'src/stores/device-store';
import { supabase } from 'src/services/supabase.service';
import { ref, Ref, onBeforeUnmount, onMounted, watch, computed } from 'vue';
import { useSubscribe } from 'src/composables/subscribe';
import { Database } from 'app/lib/database.types';
import { DateTime } from 'luxon';

type FloatSwitchTypes = Database['public']['Tables']['floatswitch_status_reports']
type FloatSwitchSelect = FloatSwitchTypes['Row']
type FloatSwitchInsert = FloatSwitchTypes['Insert']

const props = defineProps<{
  deviceId: string | undefined
}>();

const deviceStore = useDeviceStore();
const signalSubscription = useSubscribe();
const isLoading = ref(true);
const float : Ref<FloatSwitchSelect | FloatSwitchInsert | null> = ref(null);
const lastChecked = computed( () : string => {
  const datetime = DateTime.fromISO(float.value?.datetime ?? '');
  return datetime.toLocaleString(DateTime.DATETIME_SHORT);
});
const color = computed( () => {
  return float.value?.status ? 'red' : 'green';
});
const status = computed( () => {
  return float.value?.status ? 'Full' : 'Not full';
})

onMounted(async () : Promise<void> => {
  await Refresh();
});

onBeforeUnmount(async () : Promise<void> => await UnsubscribeFromFloatSwitchStatus());

watch(() => props.deviceId, async () => await Refresh());

async function Refresh(): Promise<void> {
  isLoading.value = true;
  const device = deviceStore.knownDevices.find(d => d.device_id === props.deviceId);
  if (!device) return;
  float.value = (await supabase.from<'floatswitch_status_reports', FloatSwitchTypes>('floatswitch_status_reports')
    .select()
    .eq('device_id', device.id)
    .order('datetime', { ascending: false })
    .limit(1)
    .single()).data as FloatSwitchSelect | null;
  await SubscribeToFloatSwitchStatus();
  isLoading.value = false;
}

function SubscribeToFloatSwitchStatus(): void {
  if (!props.deviceId) return;
  UnsubscribeFromFloatSwitchStatus();
  signalSubscription.Subscribe(props.deviceId, 'float_status', 'floatswitch_status_reports', payload => {
    float.value = payload.new as FloatSwitchInsert;
  });
}

async function UnsubscribeFromFloatSwitchStatus(): Promise<void> {
  await signalSubscription.Unsubscribe();
}
</script>
