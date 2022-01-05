<template>
    <line-chart
        :chartData='ultraSonicData'
        :options='chartOptions'
    ></line-chart>
</template>
<script>
import LineChart from './LineChart.vue';

export default {
  name: 'WaterLevelTimeGraph',
  components: { LineChart },
  props: {
    tickData: Array,
    deviceConfig: Object,
  },
  data() {
    return {
      chartOptions: {
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
  computed: {
    ultraSonicData() {
      const labelArray = [];
      const usDataArray = [];
      this.tickData.forEach((tick) => {
        const date = new Date(tick.datetime);
        labelArray.push(date.toLocaleString());
        usDataArray.push({
          y: this.convertDistanceToGallons(tick.data['ultra-sonic']),
        });
      });
      return {
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
  },
};
</script>
