import { useDeviceStore } from 'src/stores/device-store';
import { supabase } from 'src/services/supabase.service';
import { RealtimeChannel, RealtimePostgresInsertPayload } from '@supabase/supabase-js';
import { Ref, ref } from 'vue';

const deviceStore = useDeviceStore();

export function useSubscribe() {

  const channel : Ref<RealtimeChannel | undefined> = ref(undefined);

  function Subscribe(
    deviceId : string,
    channelName : string,
    table : string,
    callback: (payload: RealtimePostgresInsertPayload<{[key: string]: unknown}>) => void
  ) : void {
    
    const device = deviceStore.knownDevices.find(d => d.device_id === deviceId);
    if (device == null) return;
    channel.value = supabase.channel(channelName).on(
      'postgres_changes',
      { event: 'INSERT', schema: 'public', table: table, filter: `device_id=eq.${device.id}`},
      callback
    )
    .subscribe( (status, err) => {
      if (err) {
        console.log(`Error: ${err}`);
      }
      switch(status) {
        case 'CLOSED':
        case 'CHANNEL_ERROR':
        case 'TIMED_OUT': {
            // TODO : Resubscribe?
            break;
        }
      }
    });
  }

  async function Unsubscribe() : Promise<void> {
    if (channel.value) await supabase.removeChannel(channel.value);
  }
  
  return { channel, Subscribe, Unsubscribe };

}
