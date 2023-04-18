<template>
  <q-page class="full-width column items-center">
    <device-select class="col" @device-selected="onDeviceSelected" />
    <device-status class="col" :show="true" :device-id="deviceId" :average-fill-time="120" />
  </q-page>
</template>

<script setup lang="ts">
import DeviceSelect from 'src/components/DeviceSelect.vue';
import DeviceStatus from 'src/components/DeviceStatus.vue';
import { useRouteBasedOnSession } from 'src/composables/routeBasedOnSession';
import { ref, Ref } from 'vue';

useRouteBasedOnSession();

const deviceId: Ref<string | undefined> = ref('');

function onDeviceSelected(value: string | undefined) : void {
  console.log(value ?? 'undefined');
  deviceId.value = value;
}
</script>
