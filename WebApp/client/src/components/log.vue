<template>
  <div class="small">
    <device-select @deviceSelected="fetchApi"/>
    <h1>
      Status: {{ status ? "Online" : "Offline (Last Online: " + lastOnline + ")" }}
    </h1>
    <h1>Current State: {{ currentState }}</h1>
    <line-chart :chart-data="dataCollection" :options="waterLevelChartOptions"></line-chart>
  </div>
</template>

<script>
import Api from '@/api/Api';
import LineChart from './LineChart.vue';
import DeviceSelect from './DeviceSelect.vue';

export default {
  name: 'Log',
  components: { LineChart, DeviceSelect },
  props: {
  },
  data() {
    return {
      lastTick: {},
      lastOnline: null,
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
      deviceConfig: null,
    };
  },
  methods: {
    async fetchApi(deviceId) {
      this.deviceConfig = await Api.fetchConfiguration(deviceId);
      this.lastTick = await Api.fetchTick(deviceId);
      this.lastTick = this.lastTick.reverse();
      this.lastOnline = this.lastTick[0].datetime;
      const meh = await Api.fetchState(deviceId);
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
            backgroundColor: '#459fff',
            data: usDataArray,
          },
        ],
      };
    },
    getRandomInt() {
      return Math.floor(Math.random() * (50 - 5 + 1)) + 5;
    },
    convertDistanceToGallons(distance) {
      // const sensorHeight = this.deviceConfig.sonicHeight; // this.inchesToCentimeters(68.75);
      // const fullHeight = this.inchesToCentimeters(62.375);
      // const diameter = this.deviceConfig.diameter; // this.inchesToCentimeters(32);
      const { sonicHeight, diameter } = this.deviceConfig;
      const measuredHeight = this.inchesToCentimeters(sonicHeight) - distance;

      return Math.round((Math.PI * (this.inchesToCentimeters(diameter) * 0.5) ** 2 * measuredHeight) / 3785); // eslint-disable-line
    },
    inchesToCentimeters(inches) {
      return inches * 2.54;
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
