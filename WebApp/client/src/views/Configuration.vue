<template>
    <div class="config">
        <device-select @deviceSelected="onDeviceSelected"/>
        <b-card-group class="group" deck>
          <b-card
            title="Running Configuration"
            style="max-width: 20rem;"
            v-if="showCurrent"
          >
            <h3>enabled: {{ config.enabled }}</h3>
            <h3>fillStart: {{ config.fillStart }}</h3>
            <h3>fillStop: {{ config.fillStop }}</h3>
            <h3>sonicHeight: {{ config.sonicHeight }}</h3>
            <h3>floatHeight: {{ config.floatHeight }}</h3>
            <h3>diameter: {{ config.diameter }}</h3>
            <h3>pumpCooldown: {{ config.pumpCooldown }}</h3>
            <h3>tickRate: {{ config.tickRate }}</h3>
          </b-card>
          <b-card
            title="Set Configuration"
            style="max-width: 20rem;"
            v-if="showForm"
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
              <b-form-group
                id="input-group-pump-cooldown"
                label="Pump Cooldown Time (minutes)"
                label-for="input-pump-cooldown"
                description="The amount of time that must be 
                  waited before turning the pump back on. (Minutes)"
              >
                <b-form-input
                  id="input-pump-cooldown"
                  v-model.number="reqConfig.pumpCooldown"
                  type="number"
                  required
                ></b-form-input>
              </b-form-group>
              <b-form-group
                id="input-group-tick-rate"
                label="Tick Rate (minutes)"
                label-for="input-tick-rate"
                description="How often to report water levels and status. (Minutes)"
              >
                <b-form-input
                  id="input-tick-rate"
                  v-model.number="reqConfig.tickRate"
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
      showCurrent: false,
      showForm: false,
      deviceId: null,
      config: {
        enabled: false,
        fillStart: 0,
        fillStop: 0,
        sonicHeight: 0,
        floatHeight: 0,
        diameter: 0,
        pumpCooldown: 0,
        tickRate: 0,
      },
      reqConfig: {
        enabled: false,
        fillStart: 0,
        fillStop: 0,
        sonicHeight: 0,
        floatHeight: 0,
        diameter: 0,
        pumpCooldown: 0,
        tickRate: 0,
      },
    };
  },
  methods: {
    async onDeviceSelected(device) {
      if (device && device !== 'null') {
        this.showForm = true;
        this.deviceId = device;
      } else {
        this.showCurrent = false;
        this.showForm = false;
        return;
      }
      const config = await Api.fetchConfiguration(device);
      if (config) {
        this.deviceId = device;
        this.config = config;
        this.onReset(null);
        this.showCurrent = true;
      } else {
        this.showCurrent = false;
        this.showToast('Missing', 'No configuration found for this device.', 'warning');
      }
    },
    async onSubmit(event) {
      event.preventDefault();
      if (this.reqConfig.sonicHeight < this.reqConfig.floatHeight) {
        this.showToast('Form', 'The float sensor must be placed below the sonic sensor.', 'warning');
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
        pumpCooldown: this.reqConfig.pumpCooldown,
        tickRate: this.reqConfig.tickRate,
      };
      console.log(await (await Api.postConfiguration(this.deviceId, postConfig)).json()); // eslint-disable-line
      this.onDeviceSelected(this.deviceId);
      this.showToast('New Configuration', 'The new configuration has been applied.', 'success');
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
      this.reqConfig.pumpCooldown = this.config.pumpCooldown;
      this.reqConfig.tickRate = this.config.tickRate;
    },
    showToast(title, message, variant) {
      this.$bvToast.toast(message, {
        title,
        variant,
      });
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
