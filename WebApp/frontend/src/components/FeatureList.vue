<template>
  <div>
    <Accordion v-show="featureList.length > 0">
      <AccordionTab header="Feature List">
        <ul style="list-style-type: none; margin: 0; padding: 0;">
          <li v-for="feature in featureList" :key="feature._id">
            <Chip style="margin-bottom: 1em;" :label="feature.display" v-tooltip="feature.description" />
          </li>
        </ul>
      </AccordionTab>
    </Accordion>
  </div>
</template>

<script setup lang="ts">
import { ref, Ref, watch, onMounted } from 'vue';
import { useRouter } from 'vue-router';
import Accordion from 'primevue/accordion';
import AccordionTab from 'primevue/accordiontab';
import Chip from 'primevue/chip';
import { useDevicesApi, UnauthorizedException } from '@/services/devices';

const router = useRouter();
const api = useDevicesApi();

const props = defineProps({
    deviceId: String,
});

const featureList: Ref<Array<any>> = ref([]);

const callApi = async (deviceId: string | undefined) => {
  if (deviceId === undefined) {
    return;
  }
  try {
      featureList.value = await api.getFeatureList(deviceId);
    } catch( e ) {
        if (e instanceof UnauthorizedException && e.code === 401) {
          router.replace('/login');
        }
    }
};

onMounted( () => {
  callApi(props.deviceId);
});

watch( () => props.deviceId, (newDeviceId) => {
  callApi(newDeviceId);
});
</script>
