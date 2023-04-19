<template>
  <q-page class="full-width column items-center">
    <device-select class="col" @device-selected="onDeviceSelected" />
    <q-card v-for="component in configuration.components" :key="component.name" style="margin: 50px;">
      <q-card-section>
        <div class="text-h1">{{ component.display }}</div>
        <div class="text-h4">{{ component.description }}</div>
      </q-card-section>
      <q-separator dark inset />
      <q-card-section>
        <template v-for="option in component.options" :key="option.name">
          <div style="margin: 5px">
            <template v-if="option.type === 'number'">
              <q-input v-model="(option as OptionNumber).value" type="number" filled :label="capitalize(option.name)" style="max-width: 200px;" :suffix="(option as OptionNumber).units + 's'" debounce="2000" @update:model-value="sendConfiguration(option)" />
              <div v-if="(option as OptionNumber).value !== (option as OptionNumber).default">
                <i>Default: {{ (option as OptionNumber).default }}</i>
                <q-btn label="Reset" @click="resetOption(option)" />
              </div>
            </template>
            <template v-if="option.type === 'boolean'">
              <q-toggle v-model="(option as OptionBoolean).value" :label="capitalize(option.name)" left-label @update:model-value="sendConfiguration(option)" />
              <div v-if="(option as OptionBoolean).value !== (option as OptionBoolean).default">
                <i>Default: {{ (option as OptionBoolean).default }}</i>
                <q-btn label="Reset" @click="resetOption(option)" />
              </div>
            </template>
            <template v-if="option.type === 'trigger'">
              <q-btn :label="capitalize(option.name)" color="primary" @click="trigger(option, component)" />
            </template>
          </div>
        </template>
      </q-card-section>
    </q-card>
  </q-page>
</template>

<script setup lang="ts">
import DeviceSelect from 'src/components/DeviceSelect.vue';
import { useDeviceStore } from 'src/stores/device-store';
import { supabase, OptionListBaseType, ComponentBase, OptionNumberListType, OptionBooleanListType } from 'src/services/supabase.service';
import { Database } from 'lib/database.types';
import { reactive, ref, Ref } from 'vue';

type OptionBase = {
  id: number,
  name: string,
  type: string,
}

type OptionNumber = OptionBase & {
  units?: string,
  default: number,
  value: number,
}

type OptionBoolean = OptionBase & {
  default: boolean,
  value: boolean,
}

type Option = OptionNumber | OptionBoolean | OptionBase

type Component = {
  name: string,
  display: string,
  description: string,
  options: Option[],
}

type Configuration = {
  components: Component[],
}

const deviceStore = useDeviceStore();

const deviceId: Ref<string | undefined> = ref('');
const configuration: Configuration = reactive({ components: [] });

async function onDeviceSelected(value: string | undefined) : Promise<void> {
  deviceId.value = value;
  await getConfiguration();
}

async function getConfiguration() {
  const device = deviceStore.knownDevices.find(d => d.device_id === deviceId.value);
  if (!device) return;
  const components = (await supabase.from<'component_list', ComponentBase>('component_list').select().eq('device_id', device.id)).data;
  if (!components) return;
  components.forEach(async componentDb => {
    const options = (await supabase.from<'option_list', OptionListBaseType>('option_list').select().eq('component_id', componentDb.id)).data;
    if (!options) return;
    const optionList : Option[] = reactive([]);
    options.forEach(async optionBase => {
      switch(optionBase.type) {
        case 'number': {
          const optionDb = (await supabase.from<'option_number_list', OptionNumberListType>('option_number_list').select().eq('option_id', optionBase.id).limit(1).single()).data;
          if (!optionDb) return;
          optionList.push({
            id: optionDb.id,
            name: optionBase.name,
            type: optionBase.type,
            units: optionDb.units ?? '',
            default: optionDb.default,
            value: optionDb.value ?? optionDb.default,
          });
          break;
        }
        case 'boolean': {
          const optionDb = (await supabase.from<'option_boolean_list', OptionBooleanListType>('option_boolean_list').select().eq('option_id', optionBase.id).limit(1).single()).data;
          if (!optionDb) return;
          optionList.push({
            id: optionDb.id,
            name: optionBase.name,
            type: optionBase.type,
            default: optionDb.default,
            value: optionDb.value ?? optionDb.default,
          });
          break;
        }
        case 'trigger': {
          optionList.push({
            id: optionBase.id,
            name: optionBase.name,
            type: optionBase.type,
          });
          break;
        }
      }
    });
    configuration.components.push({
      name: componentDb.component_name,
      display: componentDb.display,
      description: componentDb.description ?? '',
      options: optionList,
    });
  });
}

async function sendConfiguration(option: Option) {
  switch(option.type) {
    case 'number': {
      const save : Database['public']['Tables']['option_number_list']['Update'] = {
        value: (option as OptionNumber).value,
      };
      await supabase.from<'option_number_list', OptionNumberListType>('option_number_list').update(save).eq('id', option.id);
      break;
    }
    case 'boolean': {
      const save : Database['public']['Tables']['option_boolean_list']['Update'] = {
        value: (option as OptionBoolean).value,
      };
      await supabase.from<'option_boolean_list', OptionBooleanListType>('option_boolean_list').update(save).eq('id', option.id);
      break;
    }
    default: {
      break;
    }
  }
}

async function resetOption(option: Option) {
  switch(option.type) {
    case 'number': {
      const numberOption : OptionNumber = option as OptionNumber;
      numberOption.value = numberOption.default;
      const save : Database['public']['Tables']['option_number_list']['Update'] = {
        value: null,
      };
      await supabase.from<'option_number_list', OptionNumberListType>('option_number_list').update(save).eq('id', option.id);
      break;
    }
    case 'boolean': {
      const boolOption : OptionBoolean = option as OptionBoolean;
      boolOption.value = boolOption.default;
      const save : Database['public']['Tables']['option_boolean_list']['Update'] = {
        value: null,
      };
      await supabase.from<'option_boolean_list', OptionBooleanListType>('option_boolean_list').update(save).eq('id', option.id);
      break;
    }
    default: {
      break;
    }
  }
}

function trigger(option: Option, component: Component) {
  const channel = supabase.channel('triggers', {
    config: {
      broadcast: { ack: true },
    },
  });
  channel.subscribe( async (status) => {
    if (status === 'SUBSCRIBED') {
      const begin = performance.now();
      await channel.send({
        type: 'broadcast',
        event: 'trigger',
        payload: { device_id: deviceId.value, component_name: component.name, trigger_name: option.name }
      });
      const end = performance.now()
      console.log(`Latency is ${end - begin} milliseconds`);
      channel.unsubscribe();
    }
  })
  
}

function capitalize(string : string) {
  return string.charAt(0).toUpperCase() + string.slice(1);
};
</script>
