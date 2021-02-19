<template>
  <div class="small">
    <b-form-select
      @change="onDeviceChange($event)"
      v-model="deviceSelected"
      :options="deviceList">
    </b-form-select>
    <h1>Status: {{ status ? "Online" : "Offline" }}</h1>
    <h1>Current State: {{ currentState }}</h1>
    <b-button variant="success" v-on:click="setEnable(true);">Enable</b-button>
    <b-button variant="danger" v-on:click="setEnable(false);">Disable</b-button>
    <line-chart :chart-data="dataCollection" :options="waterLevelChartOptions"></line-chart>
    <div>
      <!-- <p>{{ this.lastTick[0].data }}</p> -->
    </div>
  </div>
</template>

<script>
import LineChart from './LineChart.vue';

export default {
  name: 'Log',
  components: { LineChart },
  props: {
  },
  data() {
    return {
      deviceList: [],
      deviceSelected: null,
      lastTick: {},
      status: false,
      currentState: '',
      dataCollection: null,
      pumpData: null,
      stateList: null,
      waterLevelChartOptions: {
        title: {
          display: true,
          text: 'Water Level',
        },
        scales: {
          xAxes: [{
            display: true,
            scaleLabel: {
              display: true,
              labelString: 'Time',
            },
          }],
          yAxes: [{
            type: 'linear',
            position: 'left',
            display: true,
            scaleLabel: {
              display: true,
              labelString: 'Gallons',
            },
            ticks: {
              suggestedMin: 0,
              suggestedMax: 250,
            },
          }],
        },
      },
    };
  },
  methods: {
    async fetchDeviceList() {
      const prFetchList = await fetch(`http://${process.env.VUE_APP_API_ENDPOINT}/deviceList?secret=${process.env.VUE_APP_API_SECRET}`);
      this.deviceList = [{ value: null, text: 'Select a device' }];

      const deviceMap = await prFetchList.json();
      Object.values(deviceMap).forEach((device) => {
        this.deviceList.push({ value: device.id, text: device.name });
      });
    },
    onDeviceChange(event) {
      if (event) {
        this.deviceSelected = event;
        this.fetchStatus(event);
        this.fetchApi(event);
      }
    },
    async fetchStatus(deviceId) {
      if (deviceId !== null) {
        const prFetchStatus = await fetch(`http://${process.env.VUE_APP_API_ENDPOINT}/${deviceId}/status?secret=${process.env.VUE_APP_API_SECRET}`);
        const status = await prFetchStatus.json();
        this.status = status.online;
      }
    },
    async fetchApi(deviceId) {
      const logs = await fetch(`http://${process.env.VUE_APP_API_ENDPOINT}/${deviceId}/lastTick?secret=${process.env.VUE_APP_API_SECRET}`);
      this.lastTick = await logs.json();
      this.lastTick = this.lastTick.reverse();
      const currentStateFetch = await fetch(`http://${process.env.VUE_APP_API_ENDPOINT}/${deviceId}/currentState?secret=${process.env.VUE_APP_API_SECRET}`);
      const meh = await currentStateFetch.json();
      this.stateList = meh;
      if (meh.length > 0) {
        this.currentState = meh[0].data.state;
        this.fillData();
      }
    },
    fillData() {
      const labelArray = [];
      const usDataArray = [];
      let lastHour = null;
      this.lastTick.forEach((tick) => {
        const date = new Date(tick.datetime);
        if (lastHour === null || date.getHours() !== lastHour) {
          console.log(date); // eslint-disable-line
          labelArray.push(`${new Date(tick.datetime).getHours()}:00`);
          usDataArray.push(this.convertDistanceToGallons(tick.data['ultra-sonic']));
        }
        lastHour = date.getHours();
      });
      this.dataCollection = {
        labels: labelArray,
        datasets: [
          {
            label: 'Ultra Sonic',
            backgroundColor: '#f87979',
            data: usDataArray,
          },
        ],
      };
    },
    getRandomInt() {
      return Math.floor(Math.random() * (50 - 5 + 1)) + 5;
    },
    convertDistanceToGallons(distance) {
      const sensorHeight = this.inchesToCentimeters(68.75);
      // const fullHeight = this.inchesToCentimeters(62.375);
      const diameter = this.inchesToCentimeters(32);
      const measuredHeight = sensorHeight - distance;

      return Math.round((Math.PI * (diameter * 0.5) ** 2 * measuredHeight) / 3785);
    },
    inchesToCentimeters(inches) {
      return inches * 2.54;
    },
    async setEnable(enable) {
      if (this.deviceSelected !== null) {
        await fetch(`http://${process.env.VUE_APP_API_ENDPOINT}/${this.deviceSelected}/configuration?secret=${process.env.VUE_APP_API_SECRET}`, {
          method: 'POST',
          headers: {
            Accept: 'application/json',
            'Content-Type': 'application/json',
          },
          body: JSON.stringify({
            enabled: enable,
          }),
        });
      }
    },
  },
  created() {
    /*  this.fetchApi();
    setInterval(() => {
      this.fetchApi();
      this.fetchPumpData();
    }, 5000);
    */
    this.fetchDeviceList();
  },
  mounted() {
    // this.fillData();
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
