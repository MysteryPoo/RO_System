<template>
  <div v-if="show" class="flex flex-column">
    <DataTable :value="logs" :lazy="true" responsiveLayout="scroll" :paginator="true" :rows="10" stripedRows
      :totalRecords="deviceLogsCount" @page="onPage($event)" :loading="loading">
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
          <Button v-if="slotProps.data.criticality !== 'Restart'" icon="pi pi-trash" @click="clearLog(props.deviceId, slotProps.data._id)" />
        </template>
      </Column>
    </DataTable>
  </div>
</template>

<script setup lang="ts">
import { ref, Ref, defineProps, computed, watch } from 'vue';
import DataTable from 'primevue/datatable';
import Column from 'primevue/column';
import Button from 'primevue/button';
import Badge from 'primevue/badge';
import { useDevicesApi } from '@/services/devices';

interface Log {
  _id : string;
  component : string;
  data : {
    reason? : string;
    message? : string;
  }
  datetime : string;
  criticality : string;
  message : string | undefined;
  colorization : string;
}

interface IPageEvent {
  first? : number;
}

const props = defineProps({
  show: Boolean,
  deviceId: String,
});

const api = useDevicesApi();
const loading = ref(true);
const deviceLogsCount = ref(0);
const deviceLogs : Ref<Log[]> = ref([]);

const logs = computed( () => {
  const logs : Log[] = [];
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
      _id: log._id,
      component: log.component,
      datetime: new Date(log.datetime).toLocaleString('en-US', {
            day: 'numeric',
            month: 'short',
            hour: 'numeric',
            minute: '2-digit'
      }),
      criticality,
      message,
      colorization,
      data: log.data,
    });
  });
  return logs;
});

const clearLog = async (deviceId : string | undefined, logId : string) => {
  try {
    const response = await api.clearLog(deviceId, logId);
    if (response.status === 200) {
      refresh();
    }
  } catch (e) {
    console.log(e);
  }
};

const refresh = async () => {
  loading.value = true;
  deviceLogsCount.value = await api.getLogsCount(props.deviceId);
  deviceLogs.value = await api.getLogs(props.deviceId);
  loading.value = false;
};

const onPage = async (event : IPageEvent) => {
  deviceLogs.value = await api.getLogs(props.deviceId, event.first);
}

watch( () => props.deviceId, refresh);
</script>

<style scoped>
.table-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
}
</style>
