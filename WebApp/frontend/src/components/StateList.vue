<template>
  <div v-if="show">
    <DataTable :value="states" responsiveLayout="scroll" :paginator="true" :rows="10">
      <template #header>
        <div class="table-header">
          Activity
          <Button icon="pi pi-refresh" @click="refresh" />
        </div>
      </template>
      <Column v-for="col of columns" :field="col.field" :header="col.header" :key="col.field"></Column>
    </DataTable>
  </div>
</template>

<script setup lang="ts">
import { ref, defineProps, computed, watch } from 'vue';
import DataTable from 'primevue/datatable';
import Column from 'primevue/column';
import Button from 'primevue/button';

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
      datetime: new Date(state.datetime).toLocaleString('en-US', {
            day: 'numeric',
            month: 'short',
            hour: 'numeric',
            minute: '2-digit'
      }),
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

const refresh = async () => {
  deviceStates.value = await getStates(props.deviceId);
};

watch( () => props.deviceId, refresh);
</script>

<style scoped>
.table-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
}
</style>
