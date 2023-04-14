<template>
  <q-page class="full-width column items-center">
    <device-select class="col" @device-selected="onDeviceSelected" />
    <device-status :show="true" :device-id="deviceId" :average-fill-time="0" />
    <example-component
      class="col"
      title="Example component"
      active
      :todos="todos"
      :meta="meta"
    ></example-component>
  </q-page>
</template>

<script setup lang="ts">
import { Todo, Meta } from 'components/models';
import ExampleComponent from 'components/ExampleComponent.vue';
import DeviceSelect from 'src/components/DeviceSelect.vue';
import DeviceStatus from 'src/components/DeviceStatus.vue';
import { ref, Ref } from 'vue';

const todos = ref<Todo[]>([
  {
    id: 1,
    content: 'ct1'
  },
  {
    id: 2,
    content: 'ct2'
  },
  {
    id: 3,
    content: 'ct3'
  },
  {
    id: 4,
    content: 'ct4'
  },
  {
    id: 5,
    content: 'ct5'
  }
]);
const meta = ref<Meta>({
  totalCount: 1200
});

const deviceId: Ref<string | undefined> = ref('');

function onDeviceSelected(value: string | undefined) : void {
  console.log(value ?? 'undefined');
  deviceId.value = value;
}
</script>
