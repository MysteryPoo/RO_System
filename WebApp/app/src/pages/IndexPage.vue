<template>
  <q-page class="full-width column items-center">
    <device-list class="col" @device-selected="onDeviceSelected" />
    <device-status class="col" :device-id="deviceId" />
  </q-page>
</template>

<script setup lang="ts">
import DeviceList from 'src/components/DeviceList.vue';
import DeviceStatus from 'src/components/DeviceStatus/DeviceStatus.vue';
import { ref, Ref } from 'vue';

// TODO: Rename this page to Status

const deviceId: Ref<string | undefined> = ref('');

function onDeviceSelected(value: string | undefined) : void {
  deviceId.value = value;
}
</script>
