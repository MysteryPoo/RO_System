<template>
  <div v-if="show">
    <h1>Activity</h1>
    <DataTable :value="states" responsiveLayout="scroll">
      <Column v-for="col of columns" :field="col.field" :header="col.header" :key="col.field"></Column>
    </DataTable>
  </div>
</template>

<script setup lang="ts">
import { ref, defineProps, computed, watch } from 'vue';
import DataTable from 'primevue/datatable';
import Column from 'primevue/column';

const props = defineProps({
  show: Boolean,
  deviceId: String,
});

const unauthorizedMessage = "Unauthorized";
const deviceIdRequiredMessage = "No device provided";

const deviceStates = ref([]);

const columns = ref([
  {field: 'datetime', header: 'Date/Time'},
  {field: 'state', header: 'State'}
]);

const states = computed( () => {
  const stateList = [];
  deviceStates.value.forEach( (state) => {
    stateList.push({
      datetime: new Date(state.datetime),
      state: state.data.state,
    });
  });
  return stateList;
});

const getStates = async (deviceId) => {
  if (deviceId !== null) {
    const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/states`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      return response.json();
    }
    if (response.status === 401) {
      throw Error(unauthorizedMessage);
    }
  }
  throw Error(deviceIdRequiredMessage);
};

watch( () => props.deviceId, async () => {
  deviceStates.value = await getStates(props.deviceId);
});
</script>
