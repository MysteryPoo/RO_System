<template>
  <div class="small">
    <device-select @deviceSelected="fetchApi"/>
    <h1 v-if="show">
      Status: {{ status ? "Online" : `Offline (Last Online: ${lastOnline})` }}
    </h1>
    <h1 v-if="show">Current State: {{ status ? currentState : "Offline" }}</h1>
    <water-level-time-chart
      v-bind:tickData="lastTick"
      v-bind:deviceConfig="deviceConfig"
      v-if="show"
    ></water-level-time-chart>
    <time-range-selector v-if="show" @dateUpdated="updateFromDate"/>
    <resolution-selector v-if="show" @resolutionChanged="updateResolution"/>
  </div>
</template>

<script>
import Api from '@/api/Api';
import DeviceSelect from './DeviceSelect.vue';
import WaterLevelTimeChart from './WaterLevelTimeGraph.vue';
import TimeRangeSelector from './TimeRangeSelector.vue';
import ResolutionSelector from './ResolutionSelector.vue';

export default {
  name: 'Log',
  components: {
    DeviceSelect,
    WaterLevelTimeChart,
    TimeRangeSelector,
    ResolutionSelector,
  },
  data() {
    return {
      show: false,
      lastTick: [],
      lastOnline: null,
      status: false,
      currentState: '',
      stateList: null,
      graphSettings: {
        fromDate: TimeRangeSelector.getDefaults().fromDate,
        resolution: ResolutionSelector.getDefaults().resolution,
      },
      deviceId: null,
      deviceConfig: null,
      refreshInterval: null,
    };
  },
  methods: {
    async fetchApi(deviceId) {
      let shouldShow = true;
      if (deviceId && deviceId !== 'null') {
        this.deviceId = deviceId;
        this.deviceConfig = await Api.fetchConfiguration(deviceId);
        this.status = (await Api.fetchStatus(deviceId)).online;
        await this.updateGraphSettings(undefined, undefined);
        if (this.lastTick.length > 0) {
          this.lastOnline = (new Date(this.lastTick[0].datetime)).toLocaleString();
        } else {
          shouldShow = false;
          this.lastOnline = null;
        }
        this.stateList = await Api.fetchState(deviceId);
        if (this.stateList.length > 0) {
          this.currentState = this.stateList[0].data.state;
        } else {
          this.currentState = 'Unknown';
        }
      } else {
        shouldShow = false;
      }
      if (!this.refreshInterval) {
        this.refreshInterval = setInterval(() => {
          this.fetchApi(this.deviceId);
        }, 5000);
      }
      this.show = shouldShow;
    },
    async updateGraphSettings(fromDate, resolution) {
      this.graphSettings.fromDate = (fromDate || this.graphSettings.fromDate);
      this.graphSettings.resolution = (resolution || this.graphSettings.resolution);
      this.lastTick = await Api.fetchTick(
        this.deviceId,
        undefined,
        this.graphSettings.fromDate,
        this.graphSettings.resolution,
      );
    },
    updateFromDate(fromDate) {
      this.updateGraphSettings(fromDate, undefined);
    },
    updateResolution(resolution) {
      this.updateGraphSettings(undefined, resolution);
    },
  },
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
h3 {
  margin: 40px 0 0;
}
ul {
  list-style-type: none;
  padding: 0;
}
li {
  display: inline-block;
  margin: 0 10px;
}
a {
  color: #42b983;
}
.small {
  max-width: 600px;
  margin:  150px auto;
}
</style>
