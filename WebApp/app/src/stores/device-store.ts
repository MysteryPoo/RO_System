import { defineStore } from 'pinia';
import { DeviceListRow } from 'src/services/supabase.service';

export const useDeviceStore = defineStore('device', {
  state: () => ({
    deviceId: undefined as string | undefined,
    knownDevices: [] as DeviceListRow[],
    state: undefined as string | undefined, // TODO: May not be necessary
    version: undefined as string | undefined, // TODO: May not be necessary
  }),
  getters: {},
  actions: {
    save() : void {
      window.localStorage.deviceId = this.deviceId;
      window.localStorage.knownDevices = JSON.stringify(this.knownDevices);
    },
    load() : void {
      this.deviceId = window.localStorage.deviceId;
      try {
        this.knownDevices = JSON.parse(window.localStorage.knownDevices);
      } catch (e: unknown) {
        this.knownDevices = [];
      }
    }
  },
});
