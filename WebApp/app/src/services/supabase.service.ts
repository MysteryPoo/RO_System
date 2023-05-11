import { createClient } from '@supabase/supabase-js';
import { Database } from 'app/lib/database.types';

export const supabase = createClient<Database>(
  process.env.SUPABASE_URL ?? '',
  process.env.SUPABASE_ANON_KEY ?? '',
);

export type DeviceList = Database['public']['Tables']['device_list']
export type DeviceListRow = DeviceList['Row']

export type ComponentListRowType = Database['public']['Tables']['component_list']
export type ComponentListSelect = ComponentListRowType['Row']
export async function getComponents(deviceId: string) {
  if (deviceId == '') return;
  const device = (await supabase.from<'device_list', DeviceList>('device_list').select().eq('device_id', deviceId).limit(1).single()).data;
  if (device == null) return null;
  return (await supabase.from<'component_list', ComponentListRowType>('component_list').select().eq('device_id', device.id)).data;
}

export type StateRequestRowTypes = Database['public']['Tables']['state_request']
export type StateRequestSelect = StateRequestRowTypes['Row']
export type StateRequestInsert = StateRequestRowTypes['Insert']
export async function getStateRequests(deviceId: string) {
  if (deviceId == '') return;
  const device = (await supabase.from<'device_list', DeviceList>('device_list').select().eq('device_id', deviceId).limit(1).single()).data;
  if (device == null) return null;
  return (await supabase.from<'state_request', StateRequestRowTypes>('state_request').select().eq('device_id', device.id).order('datetime', { ascending: false })).data;
}

export type OptionListRowType = Database['public']['Tables']['option_list']
export type OptionListSelect = OptionListRowType['Row']
export type OptionNumberListType = Database['public']['Tables']['option_number_list']
export type OptionNumber = OptionNumberListType['Row']
export type OptionBooleanListType = Database['public']['Tables']['option_boolean_list']
export type OptionBooleanSelect = OptionBooleanListType['Row']
export type OptionBooleanInsert = OptionBooleanListType['Insert']
export type OptionBooleanUpdate = OptionBooleanListType['Update']

export type VersionRowTypes = Database['public']['Tables']['versions']
export type VersionSelect = VersionRowTypes['Row']
export type VersionInsert = VersionRowTypes['Insert']

export type RoSystemEnabledTypes = Database['public']['Tables']['rosystem_enabled_reports']
export type RoSystemEnabledSelect = RoSystemEnabledTypes['Row']
export type RoSystemEnabledInsert = RoSystemEnabledTypes['Insert']

export type WifiSignalTypes = Database['public']['Tables']['wifi_signal_reports']
export type WifiSignalSelect = WifiSignalTypes['Row']
export type WifiSignalInsert = WifiSignalTypes['Insert']

export type WifiQualityTypes = Database['public']['Tables']['wifi_quality_reports']
export type WifiQualitySelect = WifiQualityTypes['Row']
export type WifiQualityInsert = WifiQualityTypes['Insert']
