<template>
  <div v-if="show" class="flex flex-column">
    <Card style="background: #091930;">
      <template #title>
        State List
      </template>
      <template #content>
        <DataTable :value="states" :lazy="true" responsiveLayout="scroll" :paginator="true" :rows="10" stripedRows
        :totalRecords="deviceStatesCount" @page="onPage($event)" :loading="loading">
        <template #header>
          <div class="table-header">
            Activity
            <Button icon="pi pi-refresh" @click="refresh" />
          </div>
        </template>
        <template #empty>
          No state data found.
        </template>
        <template #loading>
          Loading state data. Please wait.
        </template>
        <Column v-for="col of columns" :field="col.field" :header="col.header" :key="col.field"></Column>
        <Column header="Result">
          <template #body="slotProps">
            <span v-if="slotProps.data.success" class="pi pi-check-circle" style="color: green;"></span>
            <span v-else class="pi pi-info-circle" style="color: yellow;" />
            {{ slotProps.data.failureReason }}
          </template>
        </Column>
      </DataTable>
      </template>
      <template #footer>
        <p>Average fill time: About {{ averageFillTime }} minutes</p>
      </template>
    </Card>
  </div>
</template>

<script setup lang="ts">
import { ref, defineProps, defineEmits, computed, watch, toRaw } from 'vue';
import Card from 'primevue/card';
import DataTable from 'primevue/datatable';
import Column from 'primevue/column';
import Button from 'primevue/button';
import { useDevicesApi } from '@/services/devices';

interface IDisplayState {
  datetime: string;
  state: string;
  reason: string;
  failureReason: string;
  success: boolean;
}

interface ISystemState {
  datetime: number;
  data: {
    state: string;
    requestReason: string;
    failureReason: string;
    success: boolean;
  };
}

interface IPageEvent {
  first? : number;
}

const props = defineProps({
  show: Boolean,
  deviceId: String,
});

const emit = defineEmits(['averageFillTime']);

const api = useDevicesApi();
const loading = ref(true);
const deviceStates = ref([]);
const deviceStatesCount = ref(0);

const columns = ref([
  {field: 'datetime', header: 'Date/Time'},
  {field: 'state', header: 'State'},
  {field: 'reason', header: 'Reason'},
]);

const states = computed( () => {
  const stateList : IDisplayState[] = [];
  deviceStates.value.forEach( (state : ISystemState) => {
    stateList.push({
      datetime: new Date(state.datetime).toLocaleString('en-US', {
            day: 'numeric',
            month: 'short',
            hour: 'numeric',
            minute: '2-digit'
      }),
      state: state.data.state,
      reason: state.data.requestReason,
      failureReason: state.data.failureReason,
      success: state.data.success,
    });
  });
  return stateList;
});

const averageFillTime = computed( () => {
  let sum = 0;
  let count = 0;
  let average = 0;
  let stateList : ISystemState[] = [];
  deviceStates.value.forEach( (state) => {
    stateList.push(toRaw(state));
  });
  for (let index = stateList.length; index > 0; index -= 1) {
    const state = stateList[index];
    if (state) {
      if (stateList[index].data.state === 'FILL' && 
          stateList[index - 1].data.state === 'IDLE') {
        count += 1;
        sum += (new Date(stateList[index - 1].datetime)).getTime() - (new Date(stateList[index].datetime)).getTime();
      }
    }
  }
  if (count > 0) {
    average = sum / count;
  }
  const averageAsMinutes = (average / 1000 / 60);
  emit('averageFillTime', averageAsMinutes);
  return averageAsMinutes.toFixed(0);
});

const refresh = async () => {
  loading.value = true;
  deviceStatesCount.value = await api.getStatesCount(props.deviceId);
  deviceStates.value = await api.getStates(props.deviceId);
  loading.value = false;
};

const onPage = async (event : IPageEvent) => {
  deviceStates.value = await api.getStates(props.deviceId, event.first);
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
