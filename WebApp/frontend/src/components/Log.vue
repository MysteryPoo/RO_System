<template>
  <div v-if="show">
    <h1>Error Logs</h1>
    <DataTable :value="logs" responsiveLayout="scroll">
      <Column v-for="col of columns" :field="col.field" :header="col.header" :key="col.field"></Column>
    </DataTable>
  </div>
</template>

<script setup>
import { ref, defineProps, computed, watch } from 'vue';
import DataTable from 'primevue/datatable';
import Column from 'primevue/column';

const props = defineProps({
  show: Boolean,
  deviceId: String,
});

const unauthorizedMessage = "Unauthorized";
const deviceIdRequiredMessage = "No device provided";

const deviceLogs = ref([]);

const columns = ref([
  {field: 'datetime', header: 'Date/Time'},
  {field: 'criticality', header: 'Criticality'},
  {field: 'message', header: 'Message'}
]);

const logs = computed( () => {
  const logs = [];
  deviceLogs.value.forEach((log) => {
    let colorization = '';
    if (log.component === 'INFO') {
      colorization = 'info';
    } else if (log.component === 'WARN') {
      colorization = 'warning';
    } else if (log.component === 'ERROR') {
      colorization = 'danger';
    }
    const criticality = log.component === 'system/restart' ? 'Restart' : log.component;
    const message = log.component === 'system/restart' ? log.data.reason : log.data.message;
    logs.push({
      dbKey: log._id,
      datetime: new Date(log.datetime),
      criticality,
      message,
      _rowVariant: colorization,
    });
  });
  return logs;
});

const getLogs = async (deviceId) => {
  if (deviceId !== null) {
    const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/logs`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      return response.json();
    }
    if (response.status === 401) {
      return Promise.reject(Error(unauthorizedMessage));
    }
  }
  return Promise.reject(Error(deviceIdRequiredMessage));
};

watch( () => props.deviceId, async () => {
  deviceLogs.value = await getLogs(props.deviceId);
});
</script>
