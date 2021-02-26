<template>
    <div class="config">
        <device-select @deviceSelected="onDeviceSelected"/>
        <b-card-group class="group" deck>
          <b-card
            title="Running Configuration"
            style="max-width: 20rem;"
          >
            <h3>enabled: {{ config.enabled }}</h3>
            <h3>fillStart: {{ config.fillStart }}</h3>
            <h3>fillStop: {{ config.fillStop }}</h3>
            <h3>sonicHeight: {{ config.sonicHeight }}</h3>
            <h3>floatHeight: {{ config.floatHeight }}</h3>
            <h3>diameter: {{ config.diameter }}</h3>
          </b-card>
          <p v-if="errors.length">
            <b>Please correct the following error(s):</b>
            <ul>
              <li v-for="error in errors" :key="error.id">{{ error }}</li>
            </ul>
          </p>
          <b-card
            title="Set Configuration"
            style="max-width: 20rem;"
          >
            <b-form @submit="onSubmit" @reset="onReset">
              <b-form-group
                id="input-group-enabled"
                label="System Enabled:"
                label-for="input-enabled"
                description="Whether the system should run or not."
              >
                <b-form-checkbox
                  id="input-enabled"
                  v-model="reqConfig.enabled"
                  name="enabled"
                  value="Enabled"
                  unchecked-value="Disabled"
                  switch
                >
                  <b>{{ reqConfig.enabled }}</b>
                </b-form-checkbox>
              </b-form-group>
              <b-form-group
                id="input-group-sonic-height"
                label="Sonic Sensor Height (in.)"
                label-for="input-sonic"
                description="The height above ground the ultra sonic sensor resides. (Inches)"
              >
                <b-form-input
                  id="input-sonic"
                  v-model.number="reqConfig.sonicHeight"
                  type="number"
                  step="0.001"
                  required
                ></b-form-input>
              </b-form-group>
              <b-form-group
                id="input-group-float-height"
                label="Float Switch Height (in.)"
                label-for="input-float"
                description="The height above ground the float switch registers as full. (Inches)"
              >
                <b-form-input
                  id="input-float"
                  v-model.number="reqConfig.floatHeight"
                  type="number"
                  step="0.001"
                  required
                ></b-form-input>
              </b-form-group>
              <b-form-group
                id="input-group-diameter"
                label="Tank Diameter (in.)"
                label-for="input-diameter"
                description="The diameter of the resovoir tank. (Inches)"
              >
                <b-form-input
                  id="input-diameter"
                  v-model.number="reqConfig.diameter"
                  type="number"
                  required
                ></b-form-input>
              </b-form-group>
              <b-button type="submit" variant="primary">Submit</b-button>
              <b-button type="reset" variant="danger">Reset</b-button>
            </b-form>
          </b-card>
        </b-card-group>
    </div>
</template>

<script>
import Api from '@/api/Api';
import DeviceSelect from '@/components/DeviceSelect.vue';

export default {
  name: 'Configuration',
  components: { DeviceSelect },
  props: ['deviceName'],
  data() {
    return {
      errors: [],
      deviceId: null,
      config: {
        enabled: false,
        fillStart: 0,
        fillStop: 0,
        sonicHeight: 0,
        floatHeight: 0,
        diameter: 0,
      },
      reqConfig: {
        enabled: false,
        fillStart: 0,
        fillStop: 0,
        sonicHeight: 0,
        floatHeight: 0,
        diameter: 0,
      },
    };
  },
  methods: {
    async onDeviceSelected(device) {
      const config = await Api.fetchConfiguration(device);
      if (config) {
        this.deviceId = device;
        this.config = config;
        this.onReset(null);
      }
    },
    async onSubmit(event) {
      event.preventDefault();
      this.errors = [];
      if (this.reqConfig.sonicHeight < this.reqConfig.floatHeight) {
        this.errors.push('The float sensor must be placed below the sonic sensor.');
        return;
      }

      const calculatedFillStop = this.inchesToCentimeters(6 + (this.reqConfig.sonicHeight - this.reqConfig.floatHeight)); // eslint-disable-line
      const calculatedFillStart = calculatedFillStop + this.inchesToCentimeters(12);
      const postConfig = {
        enabled: this.reqConfig.enabled === 'Enabled',
        fillStart: Math.round(calculatedFillStart),
        fillStop: Math.round(calculatedFillStop),
        sonicHeight: this.reqConfig.sonicHeight,
        floatHeight: this.reqConfig.floatHeight,
        diameter: this.reqConfig.diameter,
      };
      console.log(await (await Api.postConfiguration(this.deviceId, postConfig)).json()); // eslint-disable-line
      this.onDeviceSelected(this.deviceId);
    },
    async onReset(event) {
      if (event !== null) {
        event.preventDefault();
      }
      this.reqConfig.enabled = this.config.enabled ? 'Enabled' : 'Disabled';
      this.reqConfig.fillStart = this.config.fillStart;
      this.reqConfig.fillStop = this.config.fillStop;
      this.reqConfig.sonicHeight = this.config.sonicHeight;
      this.reqConfig.floatHeight = this.config.floatHeight;
      this.reqConfig.diameter = this.config.diameter;
    },
    inchesToCentimeters(inches) {
      return inches * 2.54;
    },
  },
};
</script>

<style scoped>
.group {
  display: flex;
  justify-content: center;
  align-items: center;
}
</style>
