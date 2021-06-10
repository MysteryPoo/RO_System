<template>
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
</template>

<script>
export default {
  name: 'TimeRangeSelector',
  data() {
    const fromDateCalculated = new Date((new Date()).getTime() - (7 * 24 * 60 * 60 * 1000));
    return {
      fromDate: fromDateCalculated,
      fromTime: `${fromDateCalculated.getHours()}:${fromDateCalculated.getMinutes()}:${fromDateCalculated.getSeconds()}`,
    };
  },
  methods: {
    updateFrom() {
      this.addTimeToDate();
      this.$emit('dateUpdated', this.fromDate);
    },
    addTimeToDate() {
      const breakdown = this.fromTime.split(':');
      const hours = Number(breakdown[0]);
      const minutes = Number(breakdown[1]);
      const seconds = Number(breakdown[2]);
      this.fromDate.setHours(hours, minutes, seconds);
    },
  },
  getDefaults() {
    return {
      fromDate: this.fromDate,
    };
  },
};
</script>
