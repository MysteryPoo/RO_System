<template>
  <b-container fluid>
    <b-table
      hover
      small
      :items="logs"
      :fields="fields"
    >
      <template #cell(actions)="row">
        <b-button size="sm" @click="clearLog(deviceId, row.item.dbKey);" class="mr-1">
          X
        </b-button>
      </template>
    </b-table>
  </b-container>
</template>

<script>
import Api from '@/api/Api';

export default {
  name: 'Log',
  props: {
    logData: Array,
    deviceId: String,
  },
  data() {
    return {
      fields: [
        {
          key: 'datetime',
          sortable: true,
        },
        {
          key: 'criticality',
          sortable: true,
        },
        {
          key: 'message',
          sortable: false,
        },
        {
          key: 'actions',
        },
      ],
    };
  },
  methods: {
    async clearLog(deviceId, logId) {
      await Api.clearLog(deviceId, logId);
      this.$emit('refreshLog');
    },
  },
  computed: {
    logs() {
      const logs = [];
      this.logData.forEach((log) => {
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
    },
  },
};
</script>
