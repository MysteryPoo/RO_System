<template>
  <div class="home">
    <img alt="Vue logo" src="../assets/logo.png">
    <device-select @deviceSelected="fetchApi"/>
    <h1 v-if="show">
      Status: {{ status ? "Online" : `Offline (Last Online: ${lastOnline})` }}
    </h1>
    <h1 v-if="show">Current State: {{ status ? currentState : "Offline" }}</h1>
    <water-level-graph
      v-bind:tickData="lastTick"
      v-bind:deviceConfig="deviceConfig"
      v-if="show"
    />
    <water-level-time-chart
      v-bind:tickData="lastTick"
      v-bind:deviceConfig="deviceConfig"
      v-if="show"
    ></water-level-time-chart>
    <time-range-selector v-if="show" @dateUpdated="updateFromDate"/>
    <resolution-selector v-if="show" @resolutionChanged="updateResolution"/>
    <log
      v-if="show"
      v-bind:logData="logData"
      v-bind:deviceId="deviceId"
      @refreshLog="refreshLog"
    />
  </div>
</template>

<script>
// @ is an alias to /src
import Api from '@/api/Api';
import DeviceSelect from '@/components/DeviceSelect.vue';
import WaterLevelTimeChart from '@/components/WaterLevelTimeGraph.vue';
import TimeRangeSelector from '@/components/TimeRangeSelector.vue';
import ResolutionSelector from '@/components/ResolutionSelector.vue';
import WaterLevelGraph from '@/components/WaterLevelGraph.vue';
import Log from '@/components/Log.vue';

export default {
  name: 'Home',
  components: {
    DeviceSelect,
    WaterLevelTimeChart,
    TimeRangeSelector,
    ResolutionSelector,
    WaterLevelGraph,
    Log,
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
      logData: [],
    };
  },
  mounted() {
    if (window.localStorage.token === 'null') {
      this.$router.push('Login');
    }
  },
  methods: {
    async fetchApi(deviceId) {
      let shouldShow = true;
      if (deviceId && deviceId !== 'null'
      && window.localStorage.token !== 'null') {
        this.deviceId = deviceId;
        try {
          this.deviceConfig = await Api.fetchConfiguration(deviceId);
          this.status = (await Api.fetchStatus(deviceId)).online;
          await this.updateGraphSettings(undefined, undefined);
          await this.refreshLog();
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
        } catch (err) {
          if (err.message === 'Unauthorized') {
            this.$router.push('Login');
          }
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
    async refreshLog() {
      this.logData = await Api.fetchLogs(this.deviceId);
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
.home {
  max-width: 600px;
  margin:  150px auto;
}
</style>
