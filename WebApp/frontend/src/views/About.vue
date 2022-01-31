<template>
  <div class="about">
    <h1>Frontend Version: {{ version.frontend }}</h1>
    <h1>Backend Version: {{ version.backend }}</h1> 
  </div>
</template>

<script lang="ts" setup>
import { onMounted, reactive } from 'vue';
import { useStore } from 'vuex';
import { useGenericAPI } from '@/services/generic';

const store = useStore();
const api = useGenericAPI();

const version = reactive({
  frontend: store.state.version,
  backend: 'Unknown',
});

const getBackendVersion = async (): Promise<string> => {
  return api.getVersion();
}

onMounted( async () => {
  version.backend = await getBackendVersion();
});
</script>
