<template>
  <div class="flex flex-column">
    <Toast />
    <DeviceSelect @deviceSelected="onDeviceSelected" class="flex align-items-center justify-content-center m-2 px-5 py-3 border-round" />
    <div class="card">
      <div class="flex flex-row flex-wrap justify-content-center">
        <Card v-for="feature in featureList" key="feature._id" class="Card">
          <template #title>
            {{ feature.display }}
          </template>
          <template #content>
            <h1>{{ feature.description }}</h1>
            <template v-for="option in feature.options" key="option.name">
              <template v-if="option.type === 'boolean'">
                <h2>{{ capitalize(option.name) }}</h2>
                <InputSwitch v-model="configuration[feature.component][option.name]" />
              </template>
              <template v-if="option.type === 'number'">
                <div class="flex justify-content-center" style="margin-top: 5em;">
                  <label :for="`${feature.component}_${option.name}`">{{ capitalize(option.name) }}</label>
                  <InputNumber :id="`${feature.component}_${option.name}`" v-model="configuration[feature.component][option.name]" :suffix="` ${getSuffix(configuration[feature.component][option.name], option.units)}`"/>
                </div>
              </template>
              <template v-if="option.type === 'trigger'">
                <div class="flex justify-content-center" style="margin-top: 5em;">
                  <Button @click="trigger(feature, option.name)" :label="capitalize(option.name)" class="p-button-rounded" />
                </div>
              </template>
            </template>
            <div style="margin-top: 5em;">
              <Button @click="setConfiguration(feature)" label="Submit"/>
            </div>
          </template>
        </Card>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { Ref, ref, onMounted, toRaw } from 'vue';
import { useRouter } from 'vue-router';
import { useStore } from 'vuex';
import Card from 'primevue/card';
import InputNumber from 'primevue/inputnumber';
import InputSwitch from 'primevue/inputswitch';
import Toast from 'primevue/toast';
import Button from 'primevue/button';
import { useToast } from 'primevue/usetoast';
import DeviceSelect from '@/components/DeviceSelect.vue';
import { useDevicesApi, UnauthorizedException } from '@/services/devices';

interface IFeature {
  deviceId: string;
  component: string;
  display: string;
  description: string;
  options: Array<{name: string, type: string, units?: string, default?: any}>;
}

const router = useRouter();
const store = useStore();
const api = useDevicesApi();
const toast = useToast();

const show = ref(false);
const deviceId = ref('');
let configuration : any = ref({});
const featureList: Ref<Array<IFeature>> = ref([]);

const onDeviceSelected = async (_deviceId : string) : Promise<void> => {
  deviceId.value = _deviceId;
  await getConfigurationFromApi(_deviceId);
  featureList.value = await api.getFeatureList(_deviceId);
  if (featureList.value.length === 0) {
    toast.add({severity:'info', summary: 'No Configuration', detail:'This device is not configurable.', life: 10000});
  }
  for (const feature of toRaw(featureList.value)) {
    if (!(feature.component in configuration.value)) {
      toast.add({severity:'info', summary: `${feature.display}`, detail:'This component is running on default configuration settings.', life: 10000});
      const featureConfig : any = {};
      for (const option of feature.options) {
        if (option.type === 'trigger') continue;
        featureConfig[option.name] = option.default ? option.default : option.type === 'boolean' ? false : option.type === 'number' ? 0 : '';
      }
      configuration.value[feature.component] = featureConfig;
    }
  }
};

const getConfigurationFromApi = async (deviceId : string | undefined) : Promise<void> => {
  if (deviceId === undefined) {
    return;
  }
  try {
    configuration.value = await api.getConfiguration(deviceId);
    show.value = true;
  } catch (e) {
    if (e instanceof UnauthorizedException && e.code === 401) {
      router.replace('/login');
    }
    console.log(e);
  }
};

const setConfigurationFromApi = async (deviceId : string | undefined, configuration : any) : Promise<{success: boolean, message: string}> => {
  if (deviceId === undefined) {
    return {
      success: false,
      message: 'Must select a device.',
    };
  }
  try {
    return api.setConfiguration(deviceId, configuration);
  } catch (e) {
    if (e instanceof UnauthorizedException && e.code === 401) {
      router.replace('/login');
    }
    console.log(e);
    return {
      success: false,
      message: 'Unauthorized',
    }
  }
}

const setConfiguration = async (feature : IFeature) : Promise<void> => {
  const config : any = {};
  config[feature.component] = configuration.value[feature.component];
  const response = await setConfigurationFromApi(deviceId.value, config);
  if (response.success) {
    toast.add({severity:'success', summary: 'Configuration Updated', detail: `${feature.display} configuration was updated.`, life: 3000});
  } else {
    toast.add({severity:'error', summary: 'Configuration Failed', detail: response.message, life: 6000});
  }
};

const capitalize = (string : string) => {
  return string.charAt(0).toUpperCase() + string.slice(1);
};

const getSuffix = (amount : number, unit : string | undefined) => {
  if (undefined === unit) {
    return '';
  }
  if (amount === 1) {
    return unit;
  } else {
    return `${unit}s`;
  }
};

const trigger = async (feature : IFeature, option : string) => {
  await api.triggerOption(deviceId.value, feature.component, option);
  toast.add({severity:'success', summary: 'Triggered', detail: `${feature.display} -> ${option} was triggered.`, life: 3000});
};

onMounted( () => {
  if (store.state.token === undefined) {
    router.replace('/login');
  }
});

</script>

<style>
.Card {
  width: 25rem;
  margin-bottom: 2em;
  margin-right: 2em;
  background: linear-gradient(to bottom right, rgb(27, 34, 99), rgb(17, 20, 48));
}
</style>
