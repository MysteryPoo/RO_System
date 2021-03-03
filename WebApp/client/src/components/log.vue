<template>
  <div class="small">
    <device-select @deviceSelected="fetchApi"/>
    <h1 v-if="show">
      Status: {{ status ? "Online" : `Offline (Last Online: ${lastOnline})` }}
    </h1>
    <h1 v-if="show">Current State: {{ currentState }}</h1>
    <b-container>
      <b-row>
        <b-col>
          <label for="fromDate">Choose a date</label>
          <b-form-datepicker
            id="fromDate"
            @input="updateFrom"
            value-as-date
            v-model="fromDate"
            class="mb-2">
          </b-form-datepicker>
        </b-col>
        <b-col>
          <label for="fromTime">Choose a time</label>
          <b-form-timepicker
            id="fromTime"
            @input="updateFrom"
            v-model="fromTime"
            class="mb-2">
          </b-form-timepicker>
        </b-col>
      </b-row>
    </b-container>
    <label for="resolution">Resolution</label>
    <b-form-spinbutton
      id="resolution"
      @change="updateResolution"
      v-model="resolution"
      min="10"
      max="100">
    </b-form-spinbutton>
    <line-chart
      :chart-data="dataCollection"
      :options="waterLevelChartOptions"
      v-if="show"
    ></line-chart>
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
    const fromDateCalculated = new Date((new Date()).getTime() - (7 * 24 * 60 * 60 * 1000));
    return {
      show: false,
      lastTick: {},
      lastOnline: null,
      status: false,
      currentState: '',
      stateList: null,
      dataCollection: null,
      fromDate: fromDateCalculated,
      fromTime: `${fromDateCalculated.getHours()}:${fromDateCalculated.getMinutes()}:${fromDateCalculated.getSeconds()}`,
      resolution: 10,
      deviceId: null,
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
      if (deviceId && deviceId !== 'null') {
        this.deviceId = deviceId;
        this.show = true;
        this.deviceConfig = await Api.fetchConfiguration(deviceId);
        this.status = await Api.fetchStatus(deviceId);
        this.lastTick = await Api.fetchTick(deviceId);
        this.lastTick = this.lastTick.reverse();
        if (this.lastTick.length > 0) {
          this.lastOnline = (new Date(this.lastTick[0].datetime)).toLocaleString();
          this.fillData();
        } else {
          this.show = false;
          this.lastOnline = null;
        }
        this.stateList = await Api.fetchState(deviceId);
        if (this.stateList.length > 0) {
          this.currentState = this.stateList[0].data.state;
        } else {
          this.show = false;
        }
      } else {
        this.show = false;
      }
    },
    fillData() {
      const labelArray = [];
      const usDataArray = [];
      this.lastTick.forEach((tick) => {
        const date = new Date(tick.datetime);
        labelArray.push(date.toLocaleString());
        usDataArray.push({
          y: this.convertDistanceToGallons(tick.data['ultra-sonic']),
        });
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
    async updateFrom() {
      this.addTimeToDate();
      this.lastTick = await Api.fetchTick(this.deviceId, undefined, this.fromDate, this.resolution);
      this.lastTick = this.lastTick.reverse();
      this.fillData();
    },
    addTimeToDate() {
      const breakdown = this.fromTime.split(':');
      const hours = Number(breakdown[0]);
      const minutes = Number(breakdown[1]);
      const seconds = Number(breakdown[2]);
      this.fromDate.setHours(hours, minutes, seconds);
      console.log(this.fromDate);
    },
    async updateResolution(value) {
      this.lastTick = await Api.fetchTick(this.deviceId, undefined, this.fromDate, value);
      this.lastTick = this.lastTick.reverse();
      this.fillData();
    },
    getRandomInt() {
      return Math.floor(Math.random() * (50 - 5 + 1)) + 5;
    },
    convertDistanceToGallons(distance) {
      const { sonicHeight, diameter } = this.deviceConfig;
      const measuredHeight = this.inchesToCentimeters(sonicHeight) - distance;
      const radius = this.inchesToCentimeters(diameter) * 0.5;
      const area = Math.PI * (radius ** 2);
      const scalingFactorForCentimetersSquaredGallons = 3785;

      return Math.round((area * measuredHeight) / scalingFactorForCentimetersSquaredGallons);
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
