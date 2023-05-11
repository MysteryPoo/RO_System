<template>
  <q-page class="full-width column items-center">
    <device-list class="col" @device-selected="onDeviceSelected" />
    <log-display class="col" :device-id="deviceId" />
  </q-page>
</template>

<script setup lang="ts">
import DeviceList from 'src/components/DeviceList.vue';
import LogDisplay from 'src/components/LogDisplay.vue';
import { ref, Ref } from 'vue';

const deviceId: Ref<string | undefined> = ref('');

function onDeviceSelected(value: string | undefined) : void {
  deviceId.value = value;
}
</script>
