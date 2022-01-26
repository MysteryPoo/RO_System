<template>
  <div class="flex flex-column">
    <Toast />
    <DeviceSelect @deviceSelected="onDeviceSelected" class="flex align-items-center justify-content-center m-2 px-5 py-3 border-round" />
    <div v-show="show">
      <div>
        <h2>System Enabled:</h2>
        <InputSwitch v-model="configuration.enabled" />
      </div>
      <div class="flex justify-content-center" style="margin-top: 5em;">
        <label for="flushtime">Flush Time</label>
        <InputNumber id="flushtime" v-model="configuration.flushDuration" :suffix="` ${configuration.flushDuration === 1 ? 'minute' : 'minutes'}`"/>
      </div>
      <div class="flex justify-content-center" style="margin-top: 5em;">
        <label for="pumpcooldown">Pump Cooldown</label>
        <InputNumber id="pumpcooldown" v-model="configuration.pumpCooldown" :suffix="` ${configuration.pumpCooldown === 1 ? 'minute' : 'minutes'}`"/>
      </div>
      <div class="flex justify-content-center" style="margin-top: 5em;">
        <label for="tick">Heartbeat Rate</label>
        <InputNumber id="tick" v-model="configuration.tickRate" :suffix="` ${configuration.tickRate === 1 ? 'minute' : 'minutes'}`"/>
      </div>
      <div style="margin-top: 5em;">
        <Button @click="setConfiguration" label="Submit"/>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { reactive, ref, Ref, onMounted, toRaw } from 'vue';
import { useRouter } from 'vue-router';
import { useStore } from 'vuex';
import InputNumber from 'primevue/inputnumber';
import InputSwitch from 'primevue/inputswitch';
import Toast from 'primevue/toast';
import Button from 'primevue/button';
import { useToast } from 'primevue/usetoast';
import DeviceSelect from '@/components/DeviceSelect.vue';
import { useDevicesApi, UnauthorizedException } from '@/services/devices';

interface IConfiguration {
  enabled: boolean;
  pumpCooldown: number;
  flushDuration: number;
  tickRate: number;
}

enum CONVERSION_MODE {
  TORAW,
  TODISPLAY,
}

const router = useRouter();
const store = useStore();
const api = useDevicesApi();
const toast = useToast();

const show = ref(false);
const deviceId = ref('');
let configuration : IConfiguration = reactive({
  enabled: true,
  pumpCooldown: 600000, // 10 minutes
  flushDuration: 300000, // 5 minutes
  tickRate: 60000, // 1 minute
});

const onDeviceSelected = (_deviceId : string) : void => {
  deviceId.value = _deviceId;
  getConfigurationFromApi(_deviceId);
};

const getConfigurationFromApi = async (deviceId : string | undefined) : Promise<void> => {
  if (deviceId === undefined) {
    return;
  }
  try {
    const configRequest = await api.getConfiguration(deviceId);
    if (Object.keys(configRequest).length === 0) {
      toast.add({severity:'info', summary: 'No Configuration', detail:'This device is running on default configuration settings.', life: 3000});
    }
    const convertedConfig : IConfiguration | undefined = convertConfiguration(configRequest, CONVERSION_MODE.TODISPLAY);
    configuration.enabled = convertedConfig?.enabled ?? true;
    configuration.flushDuration = convertedConfig?.flushDuration ?? 300000;
    configuration.pumpCooldown = convertedConfig?.pumpCooldown ?? 600000;
    configuration.tickRate = convertedConfig?.tickRate ?? 60000;
    show.value = true;
  } catch (e) {
    if (e instanceof UnauthorizedException && e.code === 401) {
      router.replace('/login');
    }
    console.log(e);
  }
};

const setConfigurationFromApi = async (deviceId : string | undefined) : Promise<void> => {
  if (deviceId === undefined) {
    return;
  }
  try {
    const convertedConfig : IConfiguration | undefined = convertConfiguration(toRaw(configuration), CONVERSION_MODE.TORAW);
    if (undefined === convertedConfig) {
      toast.add({severity:'error', summary: 'Invalid Configuration', detail: 'Provided configuration is invalid.', life: 3000});
      return;
    }
    const response = await api.setConfiguration(deviceId, convertedConfig);
    if (response.success) {
      toast.add({severity:'success', summary: 'Configuration Updated', detail: `Configuration has been updated.`, life: 3000});
    } else {
      toast.add({severity:'error', summary: 'Configuration NOT Updated', detail:'Failed to update the configuration.', life: 3000});
    }
  } catch (e) {
    if (e instanceof UnauthorizedException && e.code === 401) {
      router.replace('/login');
    }
  }
};

const setConfiguration = async (event : MouseEvent) : Promise<void> => {
  setConfigurationFromApi(deviceId.value);
};

const convertConfiguration = (configuration : IConfiguration, mode : CONVERSION_MODE) : IConfiguration | undefined => {
  if (mode === CONVERSION_MODE.TODISPLAY) {
    return {
      enabled: configuration.enabled ?? true,
      pumpCooldown: configuration.pumpCooldown ? configuration.pumpCooldown / 1000 / 60 : 5,
      flushDuration: configuration.flushDuration ? configuration.flushDuration / 1000 / 60 : 5,
      tickRate: configuration.tickRate ? configuration.tickRate / 1000 / 60 : 1
    };
  } else if (mode === CONVERSION_MODE.TORAW) {
    return {
      enabled: configuration.enabled,
      pumpCooldown: configuration.pumpCooldown * 1000 * 60,
      flushDuration: configuration.flushDuration * 1000 * 60,
      tickRate: configuration.tickRate * 1000 * 60
    };
  }
  else {
    return undefined;
  }
}

onMounted( () => {
  if (store.state.token === undefined) {
    router.replace('/login');
  }
});

</script>
