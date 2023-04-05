import { defineStore } from 'pinia';

export interface IDevice {
  id? : string;
  name? : string;
}

export const useDeviceStore = defineStore('device', {
  state: () => ({
    deviceId: undefined as string | undefined,
    selectedDevice: undefined as IDevice | undefined,
    knownDevices: [] as IDevice[],
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
