<template>
  <b-container>
      <label for="resolution">Resolution</label>
      <b-form-spinbutton
        id="resolution"
        @change="updateResolution"
        v-model="resolution"
        min="10"
        max="100">
      </b-form-spinbutton>
    </b-container>
</template>

<script>
export default {
  name: 'ResolutionSelector',
  data() {
    return {
      resolution: 10,
    };
  },
  methods: {
    updateResolution() {
      this.$emit('resolutionChanged', this.resolution);
    },
  },
  getDefaults() {
    return {
      resolution: this.resolution,
    };
  },
};
</script>
