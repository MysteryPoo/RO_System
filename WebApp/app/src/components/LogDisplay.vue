<template>
  <div class="q-pa-md flex justify-center">
    <q-table
      title="Logs"
      :rows="logs ?? []"
      :columns="columns"
      row-key="id"
      :loading="isLoading">
      <template v-slot:body="props">
        <q-tr :props="props">
          <q-td key="severity" :props="props">
            <q-icon :name="GetChipIcon(props.row.severity)" :color="GetChipColor(props.row.severity)" size="32px">
              <q-tooltip anchor="center middle"
                transition-show="rotate"
                transition-hide="rotate"
              >
                {{ props.row.severity }}
              </q-tooltip>
            </q-icon>
          </q-td>
          <q-td key="datetime" :props="props">
            {{ DateTime.fromISO(props.row.datetime).toLocaleString(DateTime.DATETIME_SHORT) }}
          </q-td>
          <q-td key="message" :props="props">
            {{ props.row.message }}
          </q-td>
        </q-tr>
      </template>
    </q-table>
    <q-inner-loading :showing="isLoading">
      <q-spinner-gears size="50px" color="primary" />
    </q-inner-loading>
  </div>
</template>

<script setup lang="ts">
import { supabase } from 'src/services/supabase.service';
import { Database } from 'app/lib/database.types';
import { useDeviceStore } from 'src/stores/device-store';
import { onBeforeUnmount, onMounted, ref, Ref, watch} from 'vue';
import { useSubscribe } from 'src/composables/subscribe';
import { matAddAlert, matCreate, matError, matInfo, matWarning } from '@quasar/extras/material-icons';
import { DateTime } from 'luxon';

type LogTypes = Database['public']['Tables']['log']
type LogSelect = LogTypes['Row']
type LogInsert = LogTypes['Insert']

const props = defineProps<{
  deviceId: string | undefined
}>();

const deviceStore = useDeviceStore();
const logSubscription = useSubscribe();
const isLoading = ref(true);
const logs : Ref<LogSelect[] | LogInsert[] | null> = ref(null);
const columns = [
  {
    name: 'severity',
    required: true,
    label: 'Severity',
    //align: null,
    field: (row: LogSelect) => row.severity,
    sortable: true,
  },
  {
    name: 'datetime',
    required: true,
    label: 'Date Time',
    //align: 'left',
    field: (row: LogSelect) => row.datetime,
    sortable: true,
  },
  {
    name: 'message',
    required: true,
    label: 'Message',
    //align: null,
    field: (row: LogSelect) => row.message,
    sortable: true,
  },
];

onMounted( async () : Promise<void> => await Refresh());

onBeforeUnmount( async () => {
  await UnsubscribeFromLogs();
});

watch(() => props.deviceId, async () => await Refresh());

async function Refresh() {
  isLoading.value = true;
  const device = deviceStore.knownDevices.find(d => d.device_id === props.deviceId);
  if (!device) return;
  logs.value = (await supabase.from<'log', LogTypes>('log').select().eq('device_id', device.id).order('datetime', { ascending: false }).limit(50)).data;
  SubscribeToLogs();
  isLoading.value = false;
}

function SubscribeToLogs() : void {
  if (!props.deviceId) return;
  UnsubscribeFromLogs();
  logSubscription.Subscribe(props.deviceId, 'log-display', 'log', payload => {
    logs.value?.unshift(payload.new as LogSelect);
  })
}

async function UnsubscribeFromLogs() : Promise<void> {
  await logSubscription.Unsubscribe();
}

function GetChipColor(severity: string): string {
  switch(severity) {
    case 'TRACE': {
      return 'teal';
    }
    case 'INFO': {
      return 'primary';
    }
    case 'WARN': {
      return 'deep-orange';
    }
    case 'ERROR': {
      return 'red';
    }
    default: {
      return 'yellow';
    }
  }
}

function GetChipIcon(severity: string): string {
  switch(severity) {
    case 'TRACE': {
      return matCreate;
    }
    case 'INFO': {
      return matInfo;
    }
    case 'WARN': {
      return matWarning;
    }
    case 'ERROR': {
      return matError;
    }
    default: {
      return matAddAlert;
    }
  }
}

</script>
