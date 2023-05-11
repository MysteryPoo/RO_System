<template>
  <div>
    <div class="text-h6">Firmware Version: <q-chip color="primary">{{ version?.version ?? 'Unknown' }}</q-chip></div>
  </div>
</template>

<script setup lang="ts">
import { RealtimeChannel } from '@supabase/realtime-js';
import { useDeviceStore } from 'src/stores/device-store';
import { supabase, VersionSelect, VersionInsert, VersionRowTypes } from 'src/services/supabase.service';
import { Ref, onBeforeUnmount, onMounted, ref, watch } from 'vue';

const props = defineProps<{
  deviceId: string | undefined
}>();

const deviceStore = useDeviceStore();
const versionChannel : Ref<RealtimeChannel | undefined> = ref(undefined);
const version : Ref<VersionSelect | VersionInsert | null> = ref(null);

onMounted(async () => {
  await Refresh();
});

onBeforeUnmount(async () : Promise<void> => await UnsubscribeFromVersions());

watch(() => props.deviceId, async () => await Refresh());

async function Refresh(): Promise<void> {
  const device = deviceStore.knownDevices.find(d => d.device_id === props.deviceId);
  if (!device) return;
  version.value = (await supabase.from<'versions', VersionRowTypes>('versions').select().eq('device_id', device.id).order('inserted_at', { ascending: false }).limit(1).single()).data as VersionSelect;
  await SubscribeToVersions();
}

async function SubscribeToVersions(): Promise<void> {
  await UnsubscribeFromVersions();
  const device = deviceStore.knownDevices.find(d => d.device_id === props.deviceId);
  if (device == null) return;
  versionChannel.value = supabase.channel('versions_insert').on(
    'postgres_changes',
    { event: 'INSERT', schema: 'public', table: 'versions', filter: `device_id=eq.${device.id}`},
    async payload => {
      version.value = payload.new as VersionInsert;
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

async function UnsubscribeFromVersions(): Promise<void> {
  if (versionChannel.value) await supabase.removeChannel(versionChannel.value);
}

</script>
