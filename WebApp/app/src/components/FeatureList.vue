<template>
  <div class="q-pa-md" v-show="featureList.length > 0">
    <q-list padding bordered class="rounded-borders">
      <q-expansion-item v-for="feature in featureList" :key="feature.id"
        dense
        dense-toggle
        expand-separator
        :label="feature.display">
        <q-card>
          <q-card-section>
            {{ feature.description }}
          </q-card-section>
        </q-card>
      </q-expansion-item>
    </q-list>
  </div>
</template>

<script setup lang="ts">
import { ref, Ref, watch, onMounted } from 'vue';
import { getComponents as getComponentsFromDb, type ComponentListSelect } from 'src/services/supabase.service';

const props = defineProps({
    deviceId: String,
});

const featureList: Ref<ComponentListSelect[]> = ref([]);

onMounted( async () => {
  await getComponents();
});

watch( () => props.deviceId, async () => {
  await getComponents();
});

async function getComponents() {
  if (props.deviceId == undefined) return;
  const components = await getComponentsFromDb(props.deviceId);
  if (components == null) return;
  featureList.value = components;
}
</script>
