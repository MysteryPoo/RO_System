<template>
    <div class="device-select">
        <b-form-select
            @change="onDeviceChange($event)"
            v-model="deviceSelected"
            :options="deviceList">
        </b-form-select>
    </div>
</template>

<script>
import Api from '@/api/Api';

export default {
  name: 'DeviceSelect',
  data() {
    return {
      deviceList: [],
      deviceSelected: null,
    };
  },
  async created() {
    this.deviceList = [{ value: null, text: 'Select a device' }];

    if (window.localStorage.deviceId) {
      this.deviceSelected = window.localStorage.deviceId;
      this.$emit('deviceSelected', this.deviceSelected);
    }

    const deviceMap = await Api.fetchDeviceList();
    Object.values(deviceMap).forEach((device) => {
      this.deviceList.push({ value: device.id, text: device.name });
    });
  },
  methods: {
    onDeviceChange(device) {
      this.deviceSelected = device;
      window.localStorage.deviceId = device;
      this.$emit('deviceSelected', device);
    },
  },
};
</script>
