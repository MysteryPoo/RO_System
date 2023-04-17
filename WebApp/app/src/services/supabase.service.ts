import { createClient } from '@supabase/supabase-js';
import { Database } from 'app/lib/database.types';

export const supabase = createClient<Database>(
  process.env.SUPABASE_URL ?? '',
  process.env.SUPABASE_ANON_KEY ?? '',
);

export type DeviceList = Database['public']['Tables']['device_list']
export type DeviceListRow = DeviceList['Row']
export async function getDevices() {
  return await supabase.from<'device_list', DeviceList>('device_list').select();
}

type DevicesResponse = Awaited<ReturnType<typeof getDevices>>
export type DevicesResponseSuccess = DevicesResponse['data']
export type DevicesResponseError = DevicesResponse['error']

type HeartbeatFloatSwitch = Database['public']['Tables']['heartbeat_floatswitch']
type HeartbeatRoSystem = Database['public']['Tables']['heartbeat_rosystem']
type HeartbeatWifi = Database['public']['Tables']['heartbeat_wifi']
type HeartbeatBase = Database['public']['Tables']['heartbeat']
export type Heartbeat = {
  device: DeviceListRow,
  base: HeartbeatBase['Row'],
  floatSwitch: HeartbeatFloatSwitch['Row'] | null,
  rosystem: HeartbeatRoSystem['Row'] | null,
  wifi: HeartbeatWifi['Row'] | null,
}
export async function getHeartbeat(deviceId: string) {
  if (deviceId == '') return;
  const device = (await supabase.from<'device_list', DeviceList>('device_list').select().eq('device_id', deviceId).limit(1).single()).data;
  if (device == null) return null;
  const heartbeat = (await supabase.from<'heartbeat', HeartbeatBase>('heartbeat').select().eq('device_id', device.id).order('received_at', { ascending: false }).limit(1).single()).data;
  if (heartbeat == null) return null;
  const heartbeatFloatSwitch = (await supabase.from<'heartbeat_floatswitch', HeartbeatFloatSwitch>('heartbeat_floatswitch').select().eq('heartbeat_id', heartbeat.id).limit(1).single()).data;
  const heartbeatRoSystem = (await supabase.from<'heartbeat_rosystem', HeartbeatRoSystem>('heartbeat_rosystem').select().eq('heartbeat_id', heartbeat.id).limit(1).single()).data;
  const hearbeatWifi = (await supabase.from<'heartbeat_wifi', HeartbeatWifi>('heartbeat_wifi').select().eq('heartbeat_id', heartbeat.id).limit(1).single()).data;
  return {
    device: device,
    base: heartbeat,
    floatSwitch: heartbeatFloatSwitch,
    rosystem: heartbeatRoSystem,
    wifi: hearbeatWifi,
  } as Heartbeat;
}


type ComponentBase = Database['public']['Tables']['component_list']
export type Component = ComponentBase['Row']
export async function getComponents(deviceId: string) {
  if (deviceId == '') return;
  const device = (await supabase.from<'device_list', DeviceList>('device_list').select().eq('device_id', deviceId).limit(1).single()).data;
  if (device == null) return null;
  return (await supabase.from<'component_list', ComponentBase>('component_list').select().eq('device_id', device.id)).data;
}

type StateRequestType = Database['public']['Tables']['state_request']
export type StateRequest = StateRequestType['Row']
export async function getStateRequests(deviceId: string) {
  if (deviceId == '') return;
  const device = (await supabase.from<'device_list', DeviceList>('device_list').select().eq('device_id', deviceId).limit(1).single()).data;
  if (device == null) return null;
  return (await supabase.from<'state_request', StateRequestType>('state_request').select().eq('device_id', device.id).order('datetime', { ascending: false })).data;
}
