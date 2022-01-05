<template>
  <div>
    <DataTable :value="logs" responsiveLayout="scroll">
      <Column v-for="col of columns" :field="col.field" :header="col.header" :key="col.field"></Column>
    </DataTable>
  </div>
</template>

<script setup>
import { ref, defineProps, computed } from 'vue';
import DataTable from 'primevue/datatable';
import Column from 'primevue/column';

const props = defineProps({
    logData: Array,
});

const columns = ref([
  {field: 'datetime', header: 'Date/Time'},
  {field: 'criticality', header: 'Criticality'},
  {field: 'message', header: 'Message'}
]);

const logs = computed(() => {
  const logs = [];
  props.logData.forEach((log) => {
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
</script>
