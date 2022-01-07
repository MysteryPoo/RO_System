<template>
  <div v-if="show">
    <DataTable :value="logs" responsiveLayout="scroll" :paginator="true" :rows="10">
      <template #header>
        <div class="table-header">
          Error Logs
          <Button icon="pi pi-refresh" @click="refresh" />
        </div>
      </template>
      <Column field="datetime" header="Date/Time"></Column>
      <Column field="criticality" header="Criticality">
        <template #body="slotProps">
          <Badge class="p-mr-2" :severity="slotProps.data.colorization">{{ slotProps.data.criticality }}</Badge>
        </template>
      </Column>
      <Column field="message" header="Message"></Column>
      <Column field="actions" header="Actions">
        <template #body="slotProps">
          <Button v-if="slotProps.data.criticality !== 'Restart'" icon="pi pi-trash" @click="clearLog(props.deviceId, slotProps.data.dbKey)" />
        </template>
      </Column>
    </DataTable>
  </div>
</template>

<script setup>
import { ref, defineProps, computed, watch } from 'vue';
import DataTable from 'primevue/datatable';
import Column from 'primevue/column';
import Button from 'primevue/button';
import Badge from 'primevue/badge';

const props = defineProps({
  show: Boolean,
  deviceId: String,
});

const unauthorizedMessage = "Unauthorized";
const deviceIdRequiredMessage = "No device provided";

const deviceLogs = ref([]);

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
      datetime: new Date(log.datetime).toLocaleString('en-US', {
            day: 'numeric',
            month: 'short',
            hour: 'numeric',
            minute: '2-digit'
      }),
      criticality,
      message,
      colorization,
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

const clearLog = async (deviceId, logId) => {
  if (deviceId !== null) {
    const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/logs/${logId}`, {
      method: 'DELETE',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      return response;
    }
    if (response.status === 401) {
      throw Error(unauthorizedMessage);
    }
  }
  throw Error(deviceIdRequiredMessage);
};

const refresh = async () => {
  deviceLogs.value = await getLogs(props.deviceId);
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
