<template>
  <BarometerView :pressure="pressure?.pressure" units="Pascal" />
</template>

<script setup lang="ts">
import { useSubscribe } from 'src/composables/subscribe';
import { useDeviceStore } from 'src/stores/device-store';
import { Database } from 'app/lib/database.types';
import { Ref, computed, onBeforeUnmount, onMounted, ref, watch } from 'vue';
import { supabase } from 'src/services/supabase.service';
import BarometerView from './BarometerView.vue';

type BarometerPressureTypes = Database['public']['Tables']['barometer_pressure_reports']
type BarometerPressureSelect = BarometerPressureTypes['Row']
type BarometerPressureInsert = BarometerPressureTypes['Insert']

const props = defineProps<{
  deviceId: string | undefined
}>();

const deviceStore = useDeviceStore();
const barometerPressureSubscription = useSubscribe();
const pressure : Ref<BarometerPressureSelect | BarometerPressureInsert | null> = ref(null);
const pressureMinimum = ref(0);
const pressureMaximum = ref(100);
const waterLevel = computed( () => {
  return pressureMinimum.value / pressureMaximum.value;
});

onMounted( async () : Promise<void> => await Refresh());

onBeforeUnmount( async () => {
  await UnsubscribeFromPressure();
});

watch(() => props.deviceId, async () => await Refresh());

async function Refresh() {
  const device = deviceStore.knownDevices.find(d => d.device_id === props.deviceId);
  if (!device) return;
  pressure.value = (await supabase.from<'barometer_pressure_reports', BarometerPressureTypes>('barometer_pressure_reports').select().eq('device_id', device.id).order('datetime', { ascending: false }).limit(1).single()).data;
  await SubscribeToPressure();
}

async function SubscribeToPressure() : Promise<void> {
  if (!props.deviceId) return;
  await UnsubscribeFromPressure();
  barometerPressureSubscription.Subscribe(props.deviceId, 'barometer-pressure', 'barometer_pressure_reports', payload => {
    pressure.value = payload.new as BarometerPressureSelect;
  })
}

async function UnsubscribeFromPressure() : Promise<void> {
  await barometerPressureSubscription.Unsubscribe();
}

</script>
