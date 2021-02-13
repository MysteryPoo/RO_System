<template>
  <div class="small">
    <h1>Current State: {{ currentState }}</h1>
    <line-chart :chart-data="dataCollection"></line-chart>
    <line-chart :chart-data="pumpData"></line-chart>
    <div>
      <p>{{ this.lastTick[0].data }}</p>
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
      lastTick: {},
      currentState: '',
      dataCollection: null,
      pumpData: null,
    };
  },
  methods: {
    async fetchApi() {
      const logs = await fetch('http://192.168.1.36:4000/lastTick');
      this.lastTick = await logs.json();
      this.lastTick = this.lastTick.reverse();
      const currentStateFetch = await fetch('http://192.168.1.36:4000/currentState');
      const meh = await currentStateFetch.json();
      this.currentState = meh[0].data.state;
      this.fillData();
    },
    async fetchPumpData() {
      const pumpFetch = await fetch('http://192.168.1.36:4000/pumpState');
      const pumpJson = await pumpFetch.json();
      this.fillPumpData(pumpJson);
    },
    fillPumpData(apiResponse) {
      const labelArray = [];
      const pumpArray = [];
      const apiResponseReverse = apiResponse.reverse();
      apiResponseReverse.forEach((set) => {
        labelArray.push(set.datetime);
        if (set.data.value === 'ON') {
          pumpArray.push(1);
        } else {
          pumpArray.push(0);
        }
      });
      this.pumpData = {
        labels: labelArray,
        datasets: [
          {
            label: 'Pump',
            backgroundColor: '#4287f5',
            data: pumpArray,
          },
        ],
      };
    },
    fillData() {
      const labelArray = [];
      const usDataArray = [];
      const fsDataArray = [];
      this.lastTick.forEach((tick) => {
        labelArray.push(tick.datetime);
        usDataArray.push(tick.data['ultra-sonic']);
        fsDataArray.push(tick.data.floatSwitch);
      });
      this.dataCollection = {
        labels: labelArray,
        datasets: [
          {
            label: 'Float Switch',
            backgroundColor: '#4287f5',
            data: fsDataArray,
          }, {
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
  },
  created() {
    this.fetchApi();
    setInterval(() => {
      this.fetchApi();
      this.fetchPumpData();
    }, 5000);
  },
  mounted() {
    this.fillData();
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
